#ifndef NG_PROGRESSBAR_HPP
#define NG_PROGRESSBAR_HPP

#include "../Control.hpp"
namespace ng {

class ProgressBar : public Control {
	private:
		int m_value;
		bool hide_label;
		uint32_t bar_color;
		void OnSetStyle(std::string& style, std::string& value);
		
		struct TextLine {
			Image* tex;
			int w,h;
		};
		
		TextLine progress_text;
	public:
		ProgressBar(std::string id="");
		~ProgressBar();
		
		void Render( Point position, bool isSelected );
		
		ProgressBar* Clone();
		
		int GetValue();
		void SetValue( int value );
};
}
#endif
