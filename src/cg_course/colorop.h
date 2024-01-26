#ifndef COLOROP_H
#define COLOROP_H

#include <QColor>
#include <QtMath>

static inline QColor colorInv(const QColor& c)
{
    return QColor{
        255 - c.red(),
        255 - c.green(),
        255 - c.blue(),
    };
}


static inline QColor colorSum(const QColor& l, const QColor& r)
{
    return QColor{
        (int)std::clamp<int>(l.red() + r.red(), 0, 255),
        (int)std::clamp<int>(l.green() + r.green(), 0, 255),
        (int)std::clamp<int>(l.blue() + r.blue(), 0, 255),
    };
}

static inline QColor colorMul(const QColor& l, const QColor& r)
{
    return QColor{
        (int)std::clamp<int>(l.red() * r.red() / 255, 0, 255),
        (int)std::clamp<int>(l.green() * r.green() / 255, 0, 255),
        (int)std::clamp<int>(l.blue() * r.blue() / 255, 0, 255),
    };
}

static inline QColor colorMul(const QColor& l, qreal n)
{
    return QColor{
        (int)std::clamp<int>(l.red() * n, 0, 255),
        (int)std::clamp<int>(l.green() * n, 0, 255),
        (int)std::clamp<int>(l.blue() * n, 0, 255),
    };
}


static inline QColor csum(const QColor& l, const QColor& r)
{
    return colorSum(l, r);
}

static inline QColor cmul(const QColor& l, const QColor& r)
{
    return colorMul(l, r);
}

static inline QColor cmul(const QColor& l, qreal n)
{
    return colorMul(l, n);
}




// QColor operator+(const QColor& l, const QColor& r)
// {
//     return QColor{
//         (int)std::clamp<int>(l.red() + r.red(), 0, 255),
//         (int)std::clamp<int>(l.green() + r.green(), 0, 255),
//         (int)std::clamp<int>(l.blue() + r.blue(), 0, 255),
//     };
// }

// QColor operator*(const QColor& l, const QColor& r)
// {
//     return QColor{
//         (int)std::clamp<int>(l.red() * r.red() / 255, 0, 255),
//         (int)std::clamp<int>(l.green() * r.green() / 255, 0, 255),
//         (int)std::clamp<int>(l.blue() * r.blue() / 255, 0, 255),
//     };
// }

// QColor operator*(const QColor& l, qreal n)
// {
//     return QColor{
//         (int)std::clamp<int>(l.red() * n, 0, 255),
//         (int)std::clamp<int>(l.green() * n, 0, 255),
//         (int)std::clamp<int>(l.blue() * n, 0, 255),
//     };
// }


// QColor operator*(qreal n, const QColor& l)
// {
//     return QColor{
//         (int)std::clamp<int>(l.red() * n, 0, 255),
//         (int)std::clamp<int>(l.green() * n, 0, 255),
//         (int)std::clamp<int>(l.blue() * n, 0, 255),
//     };
// }


#endif // COLOROP_H
