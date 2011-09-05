#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

namespace MeBook{

    class RenderSettings{
    public:
        RenderSettings() : textColor(Qt::black), backgroundColor(Qt::white), lineHeight(1.5), font("Sans", 24), maxImageSize(470, 790)
        {}

        void setTextColor(QColor c){
            textColor = c;
        }

        void setBackgroundColor(QColor c){
            backgroundColor = c;
        }

        void setLineHeight(float h)
        {
            lineHeight = h;
        }

        void setFont(QFont f)
        {
            font = f;
        }

        void setMaxImageSize(int width, int height)
        {
            maxImageSize = QSize(width, height);
        }

        QColor getTextColor(){return textColor;}
        QColor getBackgroundColor(){return backgroundColor;}
        float getLineHeight(){return lineHeight;}
        QFont getFont(){return font;}
        QSize getMaxImageSize(){return maxImageSize;}

    private:
        QColor textColor;
        QColor backgroundColor;
        float lineHeight;
        QFont font;
        QSize maxImageSize;

    };

}
#endif
