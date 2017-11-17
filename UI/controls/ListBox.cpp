#include <RapidXML/rapidxml.hpp>

#include "ListBox.hpp"
namespace ng {
ListBox::ListBox() {
	setType( "listbox" );
	
	m_font_height = 13;
	m_font = Fonts::GetFont( "default", m_font_height );
	
	m_scrollbar = 0;
	m_selected_index = -1;
	m_drawscrollbar = false;
	m_scrollbar_focus = false;
	
	m_last_scroll = 0;
	m_selection_color = 0xff0414CA;
}

ListBox::~ListBox() {
	if(m_scrollbar) {
		delete m_scrollbar;
	}
}

void ListBox::Clear() {
	for(Image* t : text_lines) {
		t->Free();
	}
	text_lines.clear();
	m_items.clear();
}

void ListBox::parseXml(rapidxml::xml_node<char>* node) {
	for(;node;node=node->next_sibling()) {
		if(std::string(node->name()) == "item") {
			AddItem(node->value());
		}
	}
}

void ListBox::Render( Point pos, bool isSelected ) {
	const Rect& rect = GetRect();
	int x = rect.x + pos.x;
	int y = rect.y + pos.y;
	
	Control::Render(pos,isSelected);
	
	// draw items
	int h=0,i=0;
	int offs = getListOffset();
	for(auto it = text_lines.cbegin()+offs; it != text_lines.cend(); it++,i++) {
		if(i >= m_max_items) {
			break;
		}
		
		Size s = (*it)->GetImageSize();
		if(m_selected_index == i+offs) {
			Drawing().FillRect(  x, y + h, rect.w - (m_drawscrollbar ? m_scrollrect.w : 0), s.h, m_selection_color );
		}
		Drawing().TexRect( x+2, y+h, s.w, s.h, *it );
		h += s.h;
	}
	if(m_drawscrollbar) {
		m_scrollbar->Render( pos, false );
	}
	
	
}

void ListBox::AddItem( std::string item ) {
	
	Image* img = m_font->GetTextImage( clipText( item, GetRect().w ), 0xffffff );
	cout << "w: " << GetRect().w << "\n";
	m_items.push_back( item );
	text_lines.push_back( img );
	if(m_selected_index == -1) {
		m_selected_index = 0;
	}

	updateBox();
}

void ListBox::OnMouseDown( int mX, int mY, MouseButton which_button ) {
	m_is_mouseDown = true;
	if(m_drawscrollbar) {
		if( isOnScrollbar( mX, mY ) ) {
			m_scrollbar->OnMouseDown( mX, mY, which_button );
			m_scrollbar_focus = true;
			m_last_scroll = m_scrollbar->GetValue();
			return;
		} else {
			if(m_scrollbar_focus) {
				m_scrollbar->OnLostFocus();
				m_scrollbar_focus = false;
			}
		}
	}
	
	int tmp = (mY - GetRect().y)/m_font_height + getListOffset();
	if(tmp != m_selected_index) {
		m_selected_index = tmp;
		emitEvent( "change" );
	}
}

void ListBox::OnMouseUp( int mX, int mY, MouseButton which_button ) {
	m_is_mouseDown = false;
}

void ListBox::OnMouseMove( int mX, int mY, bool mouseState ) {

	if(m_drawscrollbar) {
		if( isOnScrollbar( mX, mY ) ) {
			m_scrollbar->OnMouseMove( mX, mY, mouseState );
			m_scrollbar_focus = true;
			m_last_scroll = m_scrollbar->GetValue();
			return;
		} else {
			if(m_scrollbar_focus) {
				m_scrollbar->OnLostFocus();
				m_scrollbar_focus = false;
			}
		}
	}
}

bool ListBox::isOnScrollbar( int &mX, int &mY ) {
	if( mX > m_scrollrect.x && mX < m_scrollrect.x+m_scrollrect.w ) {
		if( mY > m_scrollrect.y && mY < m_scrollrect.y + m_scrollrect.h ) {
			return true;
		}
	}
	return false;
}

void ListBox::OnLostFocus() {
	m_is_mouseDown = false;
	if(m_scrollbar_focus) {
		m_scrollbar->OnLostFocus();
		m_scrollbar_focus = false;
	}
}

void ListBox::onRectChange() {
	updateItemsSize();
}


void ListBox::OnLostControl() {
}

void ListBox::OnGetFocus() {
}

int ListBox::getAverageHeight() {
	int h=0,i=0;
	for(auto it = text_lines.begin(); it != text_lines.end(); it++,i++) {
		Size s = (*it)->GetImageSize();
		if(h >= GetRect().h)
			break;
		h += s.h;
	}
	if(i==0) {
		return 1;
	} else {
		return h / i;
	}
}

void ListBox::updateBox() {
	const Rect& rect = GetRect();
	m_font_height = getAverageHeight();
	m_max_items = rect.h / m_font_height;
	if(m_items.size() > m_max_items) {
		if(!m_scrollbar) {
			m_scrollbar = new ScrollBar;
			m_scrollbar->SetVertical( true );
		}
		int scrollbar_width = 10;
		m_scrollrect = { rect.x + rect.w - scrollbar_width, rect.y, scrollbar_width, rect.h };
		m_scrollbar->SetRect( m_scrollrect.x, m_scrollrect.y, m_scrollrect.w, m_scrollrect.h );
		
		m_scrollbar->SetSliderSize( std::max<int>(10, std::min<int>( ( (m_max_items*100)/m_items.size()), m_scrollrect.h - 10) ) );
		m_scrollbar->SetRange( 0, m_items.size() - m_max_items );
		m_scrollbar->SetMouseWheelConstant( std::max<int>( m_max_items/3, 1 ) );
		m_drawscrollbar = true;
	}
}

void ListBox::OnMWheel( int updown ) {
	if(m_drawscrollbar) {
		m_scrollbar->OnMWheel( updown );
		m_last_scroll = m_scrollbar->GetValue();
	}
}

int ListBox::getListOffset() {
	if(m_drawscrollbar) {
		return m_last_scroll;//std::min<int>( ((sz-m_max_items)*m_last_scroll) / 100, sz - m_max_items );
	} else return 0;
}

void ListBox::updateItemsSize() {
	std::string tmp;
	for(int i=0; i < m_items.size(); i++) {
		tmp = clipText( m_items[i], GetRect().w );
		if( tmp != m_items[i] ) {
			text_lines[i] = m_font->GetTextImage( tmp, 0xffffff );
			
		}
	}
}

std::string ListBox::clipText( std::string s, int w ) {
	int maxtext = m_font->GetMaxText( s, w );
	if( maxtext < s.size() ) {
		return s.substr( 0, maxtext-3 ) + "..";
	} else {
		return s;
	}
}

int ListBox::GetSelectedIndex() {
	return m_selected_index;
}

std::string ListBox::GetText() {
	if(m_selected_index == -1)
		return "";
	else
		return m_items[ m_selected_index ];
}

void ListBox::SetSelectedIndex( int index ) {
	m_selected_index = index;
}

void ListBox::OnSetStyle(std::string& style, std::string& value) {
	STYLE_SWITCH {
		_case("value"):
			SetSelectedIndex(std::stoi(value));
	}
}

ListBox* ListBox::Clone() {
	ListBox* lb = new ListBox();
	copyStyle(lb);
	return lb;
}

}


