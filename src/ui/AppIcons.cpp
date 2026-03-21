#include "AppIcons.h"

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

#include <cmath>

// ─── Internal helpers ─────────────────────────────────────────────────────────
namespace {

template<typename Fn>
static QIcon makeIcon(int sz, Fn&& fn) {
    QPixmap pm(sz, sz);
    pm.fill(Qt::transparent);
    {
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);
        fn(p, sz);
    }

    // Disabled state: retain original colors at 40% opacity
    // instead of Qt's default full gray desaturation.
    QPixmap pmDis(sz, sz);
    pmDis.fill(Qt::transparent);
    {
        QPainter pd(&pmDis);
        pd.setOpacity(0.4);
        pd.drawPixmap(0, 0, pm);
    }

    QIcon icon;
    icon.addPixmap(pm,    QIcon::Normal,   QIcon::Off);
    icon.addPixmap(pmDis, QIcon::Disabled, QIcon::Off);
    return icon;
}


// Draw a folder shape
static void drawFolder(QPainter& p, int sz, const QColor& body, const QColor& tab)
{
    // Tab (top-left)
    p.setPen(Qt::NoPen);
    p.setBrush(tab);
    p.drawRoundedRect(1, sz/4, sz*3/8, sz/8 + 1, 1, 1);
    // Body
    p.setBrush(body);
    p.drawRoundedRect(1, sz/4 + sz/10, sz-2, sz*13/20, 2, 2);
}

// Draw a 4-point precision crosshair (targeting reticle)
static void drawCrosshair(QPainter& p, int sz, const QColor& color)
{
    const double cx = sz / 2.0, cy = sz / 2.0;
    const double r1 = sz * 0.28, r2 = sz * 0.43;
    const double dot = sz * 0.06;
    p.setPen(QPen(color, sz * 0.07, Qt::SolidLine, Qt::RoundCap));
    // 4 tick lines at cardinal points
    p.drawLine(QPointF(cx, cy - r1), QPointF(cx, cy - r2));
    p.drawLine(QPointF(cx, cy + r1), QPointF(cx, cy + r2));
    p.drawLine(QPointF(cx - r1, cy), QPointF(cx - r2, cy));
    p.drawLine(QPointF(cx + r1, cy), QPointF(cx + r2, cy));
    // Circle
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(color, sz * 0.07));
    p.drawEllipse(QPointF(cx, cy), r1, r1);
    // Center dot
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(cx, cy), dot, dot);
}

// Draw small star dots at given positions (relative 0-1)
static void drawStarDots(QPainter& p, int sz, const QColor& col)
{
    const double pts[][2] = {{0.2,0.25},{0.7,0.15},{0.85,0.6},{0.3,0.75},{0.55,0.4}};
    p.setPen(Qt::NoPen);
    p.setBrush(col);
    for (auto& pt : pts)
        p.drawEllipse(QPointF(pt[0]*sz, pt[1]*sz), sz*0.04, sz*0.04);
}

} // namespace

// ─── Standard toolbar ─────────────────────────────────────────────────────────

QIcon AppIcons::settings(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Standard gear/cog with 8 teeth
        const double cx = sz/2.0, cy = sz/2.0;
        const double rOut = sz*0.44, rIn = sz*0.30, rHub = sz*0.13;
        const int N = 8;

        QPainterPath gear;
        for (int i = 0; i < N; ++i) {
            const double base = i * 2*M_PI / N - M_PI/2;
            const double half = M_PI / N * 0.55;
            const double a0 = base - half * 1.6;
            const double a1 = base - half;
            const double a2 = base + half;
            const double a3 = base + half * 1.6;
            auto pt = [&](double a, double r){ return QPointF(cx + r*std::cos(a), cy + r*std::sin(a)); };
            if (i == 0) gear.moveTo(pt(a0, rIn));
            else        gear.lineTo(pt(a0, rIn));
            gear.lineTo(pt(a1, rOut));
            gear.lineTo(pt(a2, rOut));
            gear.lineTo(pt(a3, rIn));
        }
        gear.closeSubpath();

        QPainterPath hub;
        hub.addEllipse(QPointF(cx, cy), rHub, rHub);
        gear = gear.subtracted(hub);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(85, 90, 100));
        p.drawPath(gear);
    });
}

QIcon AppIcons::loadImages(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Yellow-brown folder
        drawFolder(p, sz, QColor(190, 150, 40), QColor(240, 195, 60));
        // Stack of 2 mini images inside folder (offset)
        const int bx = sz*5/16, by = sz*9/20, bw = sz*11/20, bh = sz*7/16;
        // Back image (slightly offset)
        p.setPen(QPen(QColor(100,80,20), 0.5));
        p.setBrush(QColor(20, 40, 160));
        p.drawRect(bx+sz/14, by-sz/14, bw, bh);
        // Front image — dark blue with checkerboard corner
        p.setPen(QPen(QColor(80,60,10), 0.8));
        p.setBrush(QColor(10, 25, 130));
        p.drawRect(bx, by, bw, bh);
        // Star symbol on front image (white 5-point star)
        const double scx = bx + bw*0.5, scy = by + bh*0.48, sr = bh*0.30;
        QPainterPath star;
        for (int i = 0; i < 5; ++i) {
            const double ao = i*2*M_PI/5 - M_PI/2;
            const double ai = ao + M_PI/5;
            QPointF op(scx + sr*std::cos(ao), scy + sr*std::sin(ao));
            QPointF ip(scx + sr*0.38*std::cos(ai), scy + sr*0.38*std::sin(ai));
            if (i == 0) star.moveTo(op); else star.lineTo(op);
            star.lineTo(ip);
        }
        star.closeSubpath();
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 230, 80));
        p.drawPath(star);
    });
}

QIcon AppIcons::loadDark(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Dark indigo folder
        drawFolder(p, sz, QColor(35, 35, 80), QColor(55, 55, 130));
        // Dark image inside (near-black rectangle)
        const int bx = sz*5/16, by = sz*9/20, bw = sz*11/20, bh = sz*7/16;
        p.setPen(QPen(QColor(60,60,120), 0.8));
        p.setBrush(QColor(8, 8, 20));
        p.drawRect(bx, by, bw, bh);
        // Crescent moon (white/blue) on the dark image
        const double mx = bx + bw*0.52, my = by + bh*0.50, mr = bh*0.30;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(180, 200, 240));
        p.drawEllipse(QPointF(mx, my), mr, mr);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.drawEllipse(QPointF(mx + mr*0.45, my - mr*0.10), mr*0.78, mr*0.78);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    });
}

QIcon AppIcons::loadFlat(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Amber/orange folder
        drawFolder(p, sz, QColor(160, 100, 20), QColor(210, 140, 40));
        // Image rectangle inside folder
        const int bx = sz*5/16, by = sz*9/20, bw = sz*11/20, bh = sz*7/16;
        p.setPen(QPen(QColor(120,80,10), 0.8));
        // Gradient from warm white center to slightly darker edges = flat field
        QRadialGradient grad(bx + bw/2.0, by + bh/2.0, bw*0.6);
        grad.setColorAt(0.0, QColor(255, 250, 220));
        grad.setColorAt(0.6, QColor(230, 220, 180));
        grad.setColorAt(1.0, QColor(180, 165, 120));
        p.setBrush(grad);
        p.drawRect(bx, by, bw, bh);
        // "F" label in the center (orange, indicating flat)
        p.setPen(QColor(180, 100, 0));
        QFont f; f.setPixelSize(std::max(5, bh*3/5)); f.setBold(true);
        p.setFont(f);
        p.drawText(QRect(bx, by, bw, bh), Qt::AlignCenter, "F");
    });
}

QIcon AppIcons::dataReduction(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Soft red background, green precision crosshair
        p.fillRect(0, 0, sz, sz, QColor(210, 130, 130));
        p.fillRect(1, 1, sz-2, sz-2, QColor(255, 220, 220));
        drawCrosshair(p, sz, QColor(0, 150, 0));
    });
}

QIcon AppIcons::movingObjectDetect(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Soft green background, red precision crosshair
        p.fillRect(0, 0, sz, sz, QColor(110, 175, 110));
        p.fillRect(1, 1, sz-2, sz-2, QColor(210, 245, 210));
        drawCrosshair(p, sz, QColor(200, 20, 20));
    });
}

QIcon AppIcons::stackImages(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Checkerboard pattern + stacked red dots = multi-image reduction
        p.fillRect(0, 0, sz, sz, QColor(10, 30, 140));
        const QColor red(200,20,20), wht(255,255,255);
        const int q = sz/4;
        // 2×2 grid in upper half
        p.fillRect(q, 0,   q-1, q-1, wht);
        p.fillRect(0, q,   q-1, q-1, wht);
        p.fillRect(q, q,   q-1, q-1, wht);
        // Red cross
        p.setPen(QPen(red, sz*0.09));
        p.drawLine(sz/2, sz/5, sz/2, sz*4/5);
        p.drawLine(sz/5, sz/2, sz*4/5, sz/2);
        // White star dots
        drawStarDots(p, sz, wht);
    });
}

QIcon AppIcons::viewReport(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // White document with text lines + red/blue header tab
        p.fillRect(0, 0, sz, sz, Qt::transparent);
        // Document body
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(245, 245, 245));
        p.drawRect(sz/8, sz/10, sz*6/8, sz*8/10);
        // Header tab (blue)
        p.setBrush(QColor(20, 40, 180));
        p.drawRect(sz/8, sz/10, sz*6/8, sz/6);
        // Text lines (gray)
        p.setBrush(QColor(100, 100, 200));
        for (int i = 0; i < 4; ++i)
            p.drawRect(sz*3/16, sz/10 + sz/5 + i * sz/7, sz*5/8, sz/12);
        // Red left-margin line
        p.setBrush(QColor(200, 20, 20));
        p.drawRect(sz*5/16, sz/10 + sz/5, sz/16, sz*4/7);
    });
}

QIcon AppIcons::closeImages(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Stack of 3 document sheets + red X = "close image files"
        auto sheet = [&](int ox, int oy, const QColor& bg) {
            p.setPen(QPen(QColor(120,120,120), 0.8));
            p.setBrush(bg);
            p.drawRect(ox, oy, sz*13/20, sz*15/20);
            // Header stripe
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(160,185,220));
            p.drawRect(ox+1, oy+1, sz*13/20-2, sz/8);
        };
        sheet(sz*6/20, sz*3/20, QColor(235,235,235));
        sheet(sz*3/20, sz*1/10, QColor(245,245,245));
        sheet(sz/20,   1,       QColor(255,255,255));
        // Red X
        const double m = sz*0.18, e = sz*0.82;
        p.setPen(QPen(QColor(210, 30, 30), sz*0.13, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(m, m), QPointF(e, e));
        p.drawLine(QPointF(e, m), QPointF(m, e));
    });
}

QIcon AppIcons::closeAll(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Window frame (outer rect + title bar) + red X = "close all windows"
        // Back window (shadow)
        p.setPen(QPen(QColor(100,100,100), 0.8));
        p.setBrush(QColor(190,190,210));
        p.drawRect(sz*3/16, sz/8, sz*11/16, sz*11/16);
        // Front window
        p.setPen(QPen(QColor(70,70,100), 0.8));
        p.setBrush(QColor(225,225,240));
        p.drawRect(sz/16, sz/16, sz*11/16, sz*11/16);
        // Title bar
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(70, 100, 170));
        p.drawRect(sz/16+1, sz/16+1, sz*11/16-2, sz/6);
        // Red X in title bar
        const double tx = sz/16.0 + sz*11/32.0;
        const double ty = sz/16.0 + sz/12.0;
        const double ta = sz*0.07;
        p.setPen(QPen(QColor(220, 40, 40), sz*0.10, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(tx-ta, ty-ta), QPointF(tx+ta, ty+ta));
        p.drawLine(QPointF(tx+ta, ty-ta), QPointF(tx-ta, ty+ta));
    });
}

// ─── Display toolbar ──────────────────────────────────────────────────────────

QIcon AppIcons::backgroundRange(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Rainbow-colored ring (HSV color wheel) = display range/histogram
        const double cx = sz/2.0, cy = sz/2.0;
        const double r1 = sz*0.40, r2 = sz*0.24;
        const int steps = 36;
        for (int i = 0; i < steps; ++i) {
            const double a1 = (i    ) * 2*M_PI / steps - M_PI/2;
            const double a2 = (i + 1) * 2*M_PI / steps - M_PI/2;
            QColor col = QColor::fromHsvF(static_cast<float>(i) / static_cast<float>(steps), 1.0f, 1.0f);
            QPainterPath path;
            path.moveTo(cx + r1*std::cos(a1), cy + r1*std::sin(a1));
            path.lineTo(cx + r1*std::cos(a2), cy + r1*std::sin(a2));
            path.lineTo(cx + r2*std::cos(a2), cy + r2*std::sin(a2));
            path.lineTo(cx + r2*std::cos(a1), cy + r2*std::sin(a1));
            path.closeSubpath();
            p.setPen(Qt::NoPen);
            p.setBrush(col);
            p.drawPath(path);
        }
        // White center hole
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(cx, cy), r2 - 1, r2 - 1);
    });
}

QIcon AppIcons::selectMarkings(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Similar rainbow ring but with dashed selection overlay
        const double cx = sz/2.0, cy = sz/2.0;
        const double r1 = sz*0.44, r2 = sz*0.20;
        const int steps = 24;
        for (int i = 0; i < steps; ++i) {
            const double a1 = (i    ) * 2*M_PI / steps - M_PI/2;
            const double a2 = (i + 1) * 2*M_PI / steps - M_PI/2;
            QColor col = QColor::fromHsvF(static_cast<float>(i) / static_cast<float>(steps), 0.9f, 0.95f);
            QPainterPath path;
            path.moveTo(cx + r1*std::cos(a1), cy + r1*std::sin(a1));
            path.lineTo(cx + r1*std::cos(a2), cy + r1*std::sin(a2));
            path.lineTo(cx + r2*std::cos(a2), cy + r2*std::sin(a2));
            path.lineTo(cx + r2*std::cos(a1), cy + r2*std::sin(a1));
            path.closeSubpath();
            p.setPen(Qt::NoPen);
            p.setBrush(col);
            p.drawPath(path);
        }
        // Dashed green selection circle
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(0, 200, 0), 1.5, Qt::DashLine));
        p.drawEllipse(QPointF(cx, cy), r2 + 2, r2 + 2);
    });
}

QIcon AppIcons::invertDisplay(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Diagonal split: upper-right white, lower-left black
        p.fillRect(0, 0, sz, sz, Qt::black);
        QPainterPath tri;
        tri.moveTo(0, 0);
        tri.lineTo(sz, 0);
        tri.lineTo(sz, sz);
        tri.closeSubpath();
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        p.drawPath(tri);
        // Diagonal line
        p.setPen(QPen(QColor(100, 100, 100), 1));
        p.drawLine(0, 0, sz, sz);
    });
}

QIcon AppIcons::knownObjectOverlay(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Dark star field background with known object markers (red boxes)
        p.fillRect(0, 0, sz, sz, QColor(5, 5, 30));
        // Star dots (white/light blue)
        drawStarDots(p, sz, QColor(200, 220, 255));
        // Extra faint stars
        const double pts2[][2] = {{0.15,0.6},{0.9,0.35},{0.6,0.85},{0.45,0.1}};
        p.setBrush(QColor(150, 150, 200));
        p.setPen(Qt::NoPen);
        for (auto& pt : pts2)
            p.drawEllipse(QPointF(pt[0]*sz, pt[1]*sz), sz*0.025, sz*0.025);
        // Known object markers: small red boxes around some stars
        p.setPen(QPen(QColor(220, 30, 30), 1.2));
        p.setBrush(Qt::NoBrush);
        const double mks[][2] = {{0.2,0.25},{0.85,0.6}};
        for (auto& mk : mks) {
            const double mx = mk[0]*sz, my = mk[1]*sz, ms = sz*0.14;
            p.drawRect(QRectF(mx - ms, my - ms, ms*2, ms*2));
        }
    });
}

// Draw a magnifying glass (lens centered at lx,ly with radius lr, handle to bottom-right)
static void drawMagnifier(QPainter& p, int sz, double lx, double ly, double lr)
{
    // Handle
    const double hx1 = lx + lr*0.68, hy1 = ly + lr*0.68;
    const double hx2 = lx + lr*1.65, hy2 = ly + lr*1.65;
    p.setPen(QPen(QColor(60, 60, 60), sz*0.12, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(hx1, hy1), QPointF(hx2, hy2));
    // Lens fill
    p.setBrush(QColor(230, 240, 255, 210));
    p.setPen(QPen(QColor(50, 50, 60), sz*0.08));
    p.drawEllipse(QPointF(lx, ly), lr, lr);
}

QIcon AppIcons::zoomIn(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        drawMagnifier(p, sz, sz*0.38, sz*0.38, sz*0.27);
        // + inside lens
        p.setPen(QPen(QColor(30, 30, 30), sz*0.09, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(sz*0.38, sz*0.38 - sz*0.14), QPointF(sz*0.38, sz*0.38 + sz*0.14));
        p.drawLine(QPointF(sz*0.38 - sz*0.14, sz*0.38), QPointF(sz*0.38 + sz*0.14, sz*0.38));
    });
}

QIcon AppIcons::zoomOut(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        drawMagnifier(p, sz, sz*0.38, sz*0.38, sz*0.27);
        // − inside lens
        p.setPen(QPen(QColor(30, 30, 30), sz*0.09, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(sz*0.38 - sz*0.14, sz*0.38), QPointF(sz*0.38 + sz*0.14, sz*0.38));
    });
}

QIcon AppIcons::zoomFit(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Light gray background with thin border frame
        p.fillRect(0, 0, sz, sz, QColor(235, 235, 235));
        p.setPen(QPen(QColor(160, 160, 160), 0.8));
        p.setBrush(Qt::NoBrush);
        p.drawRect(1, 1, sz-2, sz-2);

        // 4 diagonal arrows pointing outward to corners
        const QColor arrowCol(50, 50, 50);
        p.setPen(QPen(arrowCol, sz*0.08, Qt::SolidLine, Qt::RoundCap));
        const double c = sz/2.0, near = sz*0.20, far = sz*0.40, tip = sz*0.11;

        auto arrow = [&](double dx, double dy) {
            // Shaft from center-near to corner-far
            QPointF from(c + dx*near, c + dy*near);
            QPointF to(c + dx*far,   c + dy*far);
            p.drawLine(from, to);
            // Arrowhead (two short lines perpendicular at tip)
            p.drawLine(to, QPointF(to.x() - dx*tip + dy*tip*0.6, to.y() - dy*tip - dx*tip*0.6));
            p.drawLine(to, QPointF(to.x() - dx*tip - dy*tip*0.6, to.y() - dy*tip + dx*tip*0.6));
        };
        const double d = 1.0/std::sqrt(2.0);
        arrow( d,  d);   // bottom-right
        arrow(-d,  d);   // bottom-left
        arrow( d, -d);   // top-right
        arrow(-d, -d);   // top-left
    });
}

// ─── Blink toolbar ────────────────────────────────────────────────────────────

QIcon AppIcons::blinkImages(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // 3 cascaded image frames + circular arrow = "cycle through images"
        // Draw 3 frames offset (back to front)
        auto frame = [&](int ox, int oy, const QColor& bg) {
            p.setPen(QPen(QColor(80, 80, 100), 0.8));
            p.setBrush(bg);
            p.drawRect(ox, oy, sz*11/16, sz*11/16);
        };
        frame(sz*4/16, sz*4/16, QColor(155, 165, 200));
        frame(sz*2/16, sz*2/16, QColor(185, 195, 220));
        frame(0,       0,       QColor(220, 225, 245));

        // Small circular arrow (cycle indicator) at bottom-right
        const double cx = sz*0.76, cy = sz*0.76, r = sz*0.17;
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(30, 130, 210), sz*0.09, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(QRectF(cx-r, cy-r, r*2, r*2), 30*16, 270*16);
        // Arrow tip
        const double ax = cx + r*std::cos(30.0*M_PI/180), ay = cy - r*std::sin(30.0*M_PI/180);
        p.setPen(QPen(QColor(30, 130, 210), sz*0.08, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(ax, ay), QPointF(ax + sz*0.10, ay - sz*0.04));
        p.drawLine(QPointF(ax, ay), QPointF(ax + sz*0.04, ay + sz*0.10));
    });
}

// Draw a modern rounded-rect button background.
static void drawBtnBg(QPainter& p, int sz, const QColor& fill, const QColor& border)
{
    const double r = sz * 0.18;
    p.setPen(QPen(border, sz * 0.05));
    p.setBrush(fill);
    p.drawRoundedRect(QRectF(sz*0.06, sz*0.06, sz*0.88, sz*0.88), r, r);
}

QIcon AppIcons::blinkStop(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Rounded rect bg (dark red-tinted) + white filled square = standard stop symbol
        drawBtnBg(p, sz, QColor(70, 25, 25), QColor(100, 40, 40));
        const double sq = sz * 0.36;
        const double cx = sz / 2.0, cy = sz / 2.0;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(220, 55, 55));
        p.drawRoundedRect(QRectF(cx - sq, cy - sq, sq*2, sq*2), sz*0.06, sz*0.06);
    });
}

QIcon AppIcons::blinkPrev(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Rounded rect bg (neutral dark) + |◄ (vertical bar + left triangle)
        drawBtnBg(p, sz, QColor(48, 52, 60), QColor(70, 75, 88));
        const double cx = sz/2.0 + sz*0.06, cy = sz/2.0;
        const double th = sz*0.30, tw = sz*0.26;
        const double bx = cx - tw*0.7, bw = sz*0.08, bh = th*1.1;
        p.setPen(Qt::NoPen);
        // Triangle pointing left
        p.setBrush(QColor(210, 215, 225));
        QPolygonF tri;
        tri << QPointF(cx + tw*0.4, cy - th)
            << QPointF(cx + tw*0.4, cy + th)
            << QPointF(cx - tw*0.9, cy);
        p.drawPolygon(tri);
        // Vertical bar on the left
        p.setBrush(QColor(210, 215, 225));
        p.drawRoundedRect(QRectF(bx - bw, cy - bh/2, bw, bh), sz*0.03, sz*0.03);
    });
}

QIcon AppIcons::blinkStep(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Rounded rect bg (neutral dark) + ►| (right triangle + vertical bar)
        drawBtnBg(p, sz, QColor(48, 52, 60), QColor(70, 75, 88));
        const double cx = sz/2.0 - sz*0.06, cy = sz/2.0;
        const double th = sz*0.30, tw = sz*0.26;
        const double bx = cx + tw*0.7, bw = sz*0.08, bh = th*1.1;
        p.setPen(Qt::NoPen);
        // Triangle pointing right
        p.setBrush(QColor(210, 215, 225));
        QPolygonF tri;
        tri << QPointF(cx - tw*0.4, cy - th)
            << QPointF(cx - tw*0.4, cy + th)
            << QPointF(cx + tw*0.9, cy);
        p.drawPolygon(tri);
        // Vertical bar on the right
        p.setBrush(QColor(210, 215, 225));
        p.drawRoundedRect(QRectF(bx, cy - bh/2, bw, bh), sz*0.03, sz*0.03);
    });
}

QIcon AppIcons::blinkPlay(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Rounded rect bg (dark green) + white ▶ triangle = modern play button
        drawBtnBg(p, sz, QColor(20, 70, 30), QColor(35, 100, 50));
        const double cx = sz/2.0 - sz*0.04, cy = sz/2.0;
        const double th = sz*0.30, tw = sz*0.32;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(100, 220, 120));
        QPolygonF tri;
        tri << QPointF(cx - tw*0.6, cy - th)
            << QPointF(cx - tw*0.6, cy + th)
            << QPointF(cx + tw*0.9, cy);
        p.drawPolygon(tri);
    });
}

// ─── Theme toggle ─────────────────────────────────────────────────────────────

QIcon AppIcons::sun(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Yellow circle + 8 rays = sun (Day mode icon)
        const double cx = sz/2.0, cy = sz/2.0;
        const double r  = sz*0.22;   // core radius
        const double r1 = sz*0.32;   // ray start
        const double r2 = sz*0.46;   // ray end
        const QColor yellow(255, 210, 40);
        const QColor yOuter(255, 170, 0);

        p.setPen(QPen(yOuter, sz*0.07, Qt::SolidLine, Qt::RoundCap));
        // 8 rays
        for (int i = 0; i < 8; ++i) {
            const double a = i * M_PI / 4.0;
            p.drawLine(QPointF(cx + r1*std::cos(a), cy + r1*std::sin(a)),
                       QPointF(cx + r2*std::cos(a), cy + r2*std::sin(a)));
        }
        // Core circle
        p.setPen(QPen(yOuter, sz*0.06));
        p.setBrush(yellow);
        p.drawEllipse(QPointF(cx, cy), r, r);
    });
}

QIcon AppIcons::moon(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        // Crescent moon = Night mode icon
        const double cx = sz/2.0, cy = sz/2.0;
        const double r  = sz*0.40;
        const QColor moonColor(160, 180, 230);
        const QColor moonEdge (100, 120, 180);

        // Full circle
        p.setPen(Qt::NoPen);
        p.setBrush(moonColor);
        p.drawEllipse(QPointF(cx, cy), r, r);

        // Punch out a slightly offset circle to create the crescent
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.drawEllipse(QPointF(cx + sz*0.16, cy - sz*0.06), r*0.82, r*0.82);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);

        // Subtle outline on the lit edge
        p.setPen(QPen(moonEdge, sz*0.04));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(cx, cy), r, r);
    });
}

QIcon AppIcons::autoTheme(int sz)
{
    return makeIcon(sz, [](QPainter& p, int sz) {
        const double cx = sz/2.0, cy = sz/2.0, r = sz*0.40;

        // Clip to circle
        QPainterPath circle;
        circle.addEllipse(QPointF(cx, cy), r, r);
        p.setClipPath(circle);

        // Left half — night (dark blue)
        p.fillRect(0, 0, sz/2, sz, QColor(20, 30, 70));
        // Right half — day (light sky)
        p.fillRect(sz/2, 0, sz/2, sz, QColor(200, 230, 255));

        p.setClipping(false);

        // Left half: small crescent moon (white)
        const double mx = sz*0.28, my = sz*0.38, mr = sz*0.17;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(200, 220, 255));
        p.drawEllipse(QPointF(mx, my), mr, mr);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.drawEllipse(QPointF(mx + mr*0.45, my - mr*0.15), mr*0.78, mr*0.78);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);

        // Right half: mini sun (yellow) — small core + 6 rays
        const double sx = sz*0.72, sy = sz*0.38, sr = sz*0.10, sray1 = sz*0.14, sray2 = sz*0.20;
        p.setPen(QPen(QColor(220, 160, 0), sz*0.05, Qt::SolidLine, Qt::RoundCap));
        for (int i = 0; i < 6; ++i) {
            const double a = i * M_PI / 3.0;
            p.drawLine(QPointF(sx + sray1*std::cos(a), sy + sray1*std::sin(a)),
                       QPointF(sx + sray2*std::cos(a), sy + sray2*std::sin(a)));
        }
        p.setPen(QPen(QColor(200, 140, 0), sz*0.04));
        p.setBrush(QColor(255, 210, 40));
        p.drawEllipse(QPointF(sx, sy), sr, sr);

        // Bottom label "A" in the middle
        const double lx = sz*0.50, ly = sz*0.78;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(120, 120, 120));
        p.drawEllipse(QPointF(lx, ly), sz*0.12, sz*0.10);
        // "A" text in white
        p.setPen(QColor(255, 255, 255));
        QFont f; f.setPixelSize(std::max(6, sz/5)); f.setBold(true);
        p.setFont(f);
        p.drawText(QRectF(lx - sz*0.12, ly - sz*0.10, sz*0.24, sz*0.20), Qt::AlignCenter, "A");

        // Circle border (dividing line + outer ring)
        p.setPen(QPen(QColor(100, 100, 100), sz*0.05));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(cx, cy), r, r);
        // Vertical dividing line
        p.setPen(QPen(QColor(100, 100, 100), sz*0.04));
        p.drawLine(QPointF(cx, cy - r), QPointF(cx, cy + r));
    });
}

static void drawMeasureIcon(QPainter& p, int sz,
                             const QColor& lineCol, const QColor& tickCol)
{
    const double cx = sz/2.0, cy = sz/2.0;
    const double r  = sz*0.40;
    const double g  = sz*0.08;

    QPen hairPen(lineCol, sz*0.07, Qt::SolidLine, Qt::RoundCap);
    p.setPen(hairPen);
    p.drawLine(QPointF(cx - r, cy), QPointF(cx - g, cy));
    p.drawLine(QPointF(cx + g, cy), QPointF(cx + r, cy));
    p.drawLine(QPointF(cx, cy - r), QPointF(cx, cy - g));
    p.drawLine(QPointF(cx, cy + g), QPointF(cx, cy + r));

    QPen tickPen(tickCol, sz*0.055, Qt::SolidLine, Qt::RoundCap);
    p.setPen(tickPen);
    const double step = (r - g) / 3.0;
    const double tk   = sz*0.10;
    for (int i = 1; i <= 2; ++i) {
        const double xp = cx + g + i * step;
        p.drawLine(QPointF(xp, cy - tk), QPointF(xp, cy + tk));
    }

    p.setPen(Qt::NoPen);
    p.setBrush(lineCol);
    p.drawEllipse(QPointF(cx, cy), sz*0.06, sz*0.06);
}

QIcon AppIcons::measure(int sz)
{
    // Off state: blue crosshair (inactive)
    QPixmap pmOff(sz, sz);
    pmOff.fill(Qt::transparent);
    {
        QPainter p(&pmOff);
        p.setRenderHint(QPainter::Antialiasing);
        drawMeasureIcon(p, sz, QColor(80, 200, 255), QColor(255, 220, 80));
    }

    // On state: amber/orange crosshair — visually signals active measure mode
    QPixmap pmOn(sz, sz);
    pmOn.fill(Qt::transparent);
    {
        QPainter p(&pmOn);
        p.setRenderHint(QPainter::Antialiasing);
        drawMeasureIcon(p, sz, QColor(255, 160, 30), QColor(255, 80, 60));
    }

    // Disabled state: 40% opacity
    QPixmap pmDis(sz, sz);
    pmDis.fill(Qt::transparent);
    {
        QPainter pd(&pmDis);
        pd.setOpacity(0.4);
        pd.drawPixmap(0, 0, pmOff);
    }

    QIcon icon;
    icon.addPixmap(pmOff, QIcon::Normal,   QIcon::Off);
    icon.addPixmap(pmOn,  QIcon::Normal,   QIcon::On);
    icon.addPixmap(pmDis, QIcon::Disabled, QIcon::Off);
    return icon;
}
