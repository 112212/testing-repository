#ifndef NG_TEXTBOX_HPP
#define NG_TEXTBOX_HPP

#include "../../Control.hpp"
#include "../../common/SDL/Drawing.hpp"
#include <vector>
namespace ng {

class TextBox : public Control {
	private:
	
		
		struct TextLine {
			unsigned int tex;
			int w,h;
			std::string text;
			TextLine() : tex(0xffffffff) {}
		};
		std::vector<TextLine> m_lines;
		
		// position of window
		Point m_position;
		// anchor for selection (no selection if x,y = -1,-1)
		Point m_anchor;
		
		Point m_cursor;
		int m_line_max;
		int m_cursor_max_x;
		int m_cursor_blink_counter;
		int m_cursor_blinking_rate;
		int m_readonly;
		
		int m_text_max;
		int m_lines_max;
		int m_line_height;
		
		static int m_selection_color;
		static int m_cursor_color;
		
		void updatePosition();
		void onFontChange();
		void updateTexture(TextLine&,bool = false);
		void sortPoints(Point &p1, Point &p2);
		void deleteSelection();
		
		// config
		bool m_multiline;
		
		// terminal mode
		bool m_terminal_mode;
		int m_terminal_max_messages;
		int m_ring_head;
				
		bool m_mousedown;

		void onPositionChange();
		void STYLE_FUNC(value);
		
	public:
	
		enum event {
			change = 0,
			enter,
			terminal_command,
			max_events
		};
	
		TextBox();
		~TextBox();
		
		//
		virtual void Render( Point position, bool isSelected );
		virtual void OnMouseDown( int mX, int mY );
		virtual void OnMouseUp( int mX, int mY );
		virtual void OnMouseMove( int mX, int mY, bool mouseState );
		virtual void OnLostFocus();
		virtual void OnGetFocus();
		virtual void OnKeyDown( SDL_Keycode &sym, SDL_Keymod mod );
		virtual void OnLostControl();
		//
				
		TextBox* Clone();
		void SetText(std::string text);
		std::string GetText();
		std::string GetSelectedText();
		
		void PutTextAtCursor(std::string text);
		void PutCursorAt( Point cursor );
		
		void SetSelection( Point start, Point end );
		void SetCursor( SDL_Cursor* curs );
		void SetMultilineMode( bool tf );
		void SetCursorBlinkingRate( int rate );
		
		void SetReadOnly( bool );
};
}
#endif
