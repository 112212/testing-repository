#include "common/ControlManager.hpp"
#include "Control.hpp"

// za interface eventa sa GUI einom
#include "Gui.hpp"
#include "Widget.hpp"

#ifdef USE_SDL
	#include "common/SDL/Drawing.hpp"
#endif

namespace ng {
Control::Control() : id("0"), engine(0), widget(0), z_index(0), type(TYPE_CONTROL), isWidget(false),
interactible(true), visible(true), anchor({{0,0},0}), m_bordercolor(0xff333376), m_hoverbordercolor(0xff1228D1), m_backcolor(0), m_font(0) {
	m_rect.x = m_rect.y = m_rect.w = m_rect.h = 0;
}

Control::~Control() {
}

// int Control::m_hoverbordercolor = 0xff1D1DBB;

const Point Control::getOffset() {
	if(engine)
		return engine->sel_widget_offset;
	else
		return {0,0};
}

void Control::SetZIndex( int zindex ) {
	if(zindex == z_index) return;
	if(widget) {
		widget->setZIndex(this, zindex);
	} else if(engine) {
		engine->setZIndex(this, zindex);
	}
}

void Control::_updateCache(CacheUpdateFlag flag) {
	if(widget) {
		widget->updateCache(this,flag);
	} else if(engine) {
		engine->updateCache(this,flag);
	}
}

bool Control::isThisControlSelected() {
	return engine && engine->selected_control == this;
}

void Control::SetVisible(bool visible) {
	if(this->visible != visible) {
		this->visible = visible;
		_updateCache(CacheUpdateFlag::attributes);
		if(!visible && isThisControlSelected() ) {
			sendGuiCommand(GUI_UNSELECT);
		}
	}
}

void Control::setInteractible(bool interactible) {
	if(this->interactible != interactible) {
		this->interactible = interactible;
		
		_updateCache(CacheUpdateFlag::attributes);
	}
}

void Control::SetId(std::string id) {
	this->id = id;
}


void Control::setType( controlType type ) {
	this->type = type;
	_updateCache(CacheUpdateFlag::all);
}

// emit global event
void Control::emitEvent( int EventID ) {
	#ifdef USE_EVENT_QUEUE
	if(engine) {
		engine->m_events.push( { id, EventID, this } );
	}
	#endif
	
	if(EventID < subscribers.size()) {
		auto &lst = subscribers[EventID];
		for(auto i = lst.begin(); i != lst.end(); i++) {
			(*i)(this);
		}
	}
}

void Control::Focus() {
	if(engine) {
		engine->Focus(this);
	}
}

void Control::SubscribeEvent( int event_type, std::function<void(Control*)> callback ) {
	if(event_type < subscribers.size())
		subscribers[event_type].push_back(callback);
}

void Control::sendGuiCommand( int eventId ) {
	if(engine) {
		engine->processControlEvent(eventId);
	}
}

const std::vector<Control*> Control::getWidgetControls() {
	if(widget)
		return widget->controls;
	else
		return std::vector<Control*>();
}
const std::vector<Control*> Control::getEngineControls() {
	if(engine)
		return engine->GetControls();
	else
		return std::vector<Control*>();
}

bool Control::check_collision(int x, int y) {
	Rect& r = m_rect;
	return (x >= r.x && x <= r.x+r.w && y >= r.y && y <= r.y+r.h);
}

void Control::SetRect( int x, int y, int w, int h ) {
	int old_x = m_rect.x;
	int old_y = m_rect.y;
	
	if(isWidget)
		((Widget*)this)->setRect(x,y,w,h);
	
	m_rect.x = x;
	m_rect.y = y;
	m_rect.w = w;
	m_rect.h = h;

	Widget* widget = getWidget();
	GuiEngine* engine = getEngine();
	if(widget) {
		widget->updateCache(this, CacheUpdateFlag::position);
	} else if(engine) {
		engine->updateCache(this, CacheUpdateFlag::position);
	}
	
	onPositionChange();
}

bool Control::custom_check = false;

void Control::SetRect( Rect r ) { 
	SetRect(r.x, r.y, r.w, r.h);
}

void Control::SetAnchor( float W, float w, float x, float H, float h, float y ) {
	this->anchor = (Anchor){ .coord = anchor.coord, .x = x, .y = y, .W = W, .w = w, .H = H, .h = h };
}

void Control::SetAnchor( Rect r ) {
	Anchor a{{0,0},0};
	a.x = r.x;
	a.y = r.y;
	a.sx = r.w;
	a.sy = r.h;
	this->anchor = a;
}

void Control::SetAnchor( const Anchor& anchor ) {
	this->anchor = anchor;
}
const Anchor& Control::GetAnchor() {
	return anchor;
}


// make these virtuals optional
void Control::OnMouseMove( int mX, int mY, bool mouseState ) {}
void Control::OnMouseDown( int mX, int mY ) {}
void Control::OnMouseUp( int mX, int mY ) {}
#ifdef USE_SFML
	void Control::Render( sf::RenderTarget& ren, sf::RenderStates state, bool isSelected ) {}
	void Control::OnKeyDown( sf::Event::KeyEvent &sym ) {}
	void Control::OnKeyUp( sf::Event::KeyEvent &sym ) {}
#elif USE_SDL
	void Control::Render( Point pos, bool selected ) {
		#ifdef SELECTION_MARK
			Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, selected ? m_hoverbordercolor : m_bordercolor );
		#else
			Drawing::Rect(m_rect.x+pos.x, m_rect.y+pos.y, m_rect.w, m_rect.h, m_bordercolor );
		#endif
	}
	void Control::OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod ) {}
	void Control::OnKeyUp(  SDL_Keycode &sym, SDL_Keymod mod ) {}
	void Control::SetFont( std::string name, int size ) { 
		TTF_Font* f = Fonts::GetFont(name, size); 
		if(f) 
			m_font = f; 
	}
#endif
void Control::OnLostFocus() {}
void Control::onPositionChange() {}
void Control::onFontChange() {}

void Control::OnGetFocus() {}
void Control::OnLostControl() {}
void Control::OnMWheel( int updown ) {}
bool Control::customBoundary( int x, int y ) {}

Control* Control::Clone() {
	return new Control;
}

void Control::SetStyle(std::string& style, std::string& value) {
	const Rect& r = GetRect();
	
	STYLE_SWITCH {
		_case("x"):
			SetRect(std::stoi(value), r.y, r.w, r.h);
		_case("y"):
			SetRect(r.x, std::stoi(value), r.w, r.h);
		_case("w"):
			SetRect(r.x, r.y, std::stoi(value), r.h);
		_case("h"):
			SetRect(r.x, r.y, r.w, std::stoi(value));
		_case("id"):
			SetId(value);
		_case("visible"):
			SetVisible(value=="true"); break;
		_case("bordercolor"):
			m_bordercolor = Colors::ParseColor(value);
		_case("background_color"):
			m_backcolor = Colors::ParseColor(value);
		_case("hoverbordercolor"):
			m_hoverbordercolor = Colors::ParseColor(value);
		_case("font"): {
			TTF_Font* f = Fonts::GetParsedFont(value);
			if(f) {
				m_font = f;
			}
			onFontChange();
		}
			break;
		default:
			OnSetStyle(style, value);
	}

}

void Control::STYLE_FUNC(value) {}

Anchor& Anchor::operator+=(const Anchor& b) {
		W += b.W;
		w += b.w;
		x += b.x;
		
		H += b.H;
		h += b.h;
		y += b.y;
		
		isrelative = isrelative ^ b.isrelative;
		ax = b.ax;
		ay = b.ay;
		
		return *this;
}

std::ostream& operator<< (std::ostream& stream, const Anchor& a) {
	return stream << a.W << ", " << a.w << ", " << a.x << " ; " <<
		a.H << ", " << a.h << ", " << a.y << " ; " <<
		a.sW<< "+" << a.sx << ", " << a.sH << "+" << a.sy << " ; (" << 
		a.coord.x << ", " << a.coord.y << ") adv (" << a.ax << ", " << 
		a.ay << ")" << " r: " << a.isrelative << endl;
}


}


