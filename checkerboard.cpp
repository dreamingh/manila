#include "checkerboard.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <cmath>

// 四个玩家的颜色
static const QColor PLAYER_COLORS[4] = {
    QColor(220, 60, 60),    // 红
    QColor(60, 140, 220),   // 蓝
    QColor(60, 180, 90),    // 绿
    QColor(230, 180, 40)    // 黄
};

CheckerBoard::CheckerBoard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("MNL - Qt 实现");
    setFixedSize(800,
                 800);

    setMouseTracking(true);

    initSlots();
    initPieces();
}

void CheckerBoard::initSlots()
{
    // 使用 7x7 网格中的 24 个点（类似直棋/九子棋布局）
    // 外圈：16 个点
    // 内圈：8 个点
    m_slots.clear();

    // 窗口 640×640，中心 (320, 320)
    // 三层方环半径（到边中心的像素距离）


    // ---- 外圈 12 点 ----
    m_slots.append({QPoint(50, 30),0});          // 港口1
    m_slots.append({QPoint(50, 80),0});          // 港口2
    m_slots.append({QPoint(50, 130),0});          // 港口3
    m_slots.append({QPoint(500, 30),0});               // 修船厂1
    m_slots.append({QPoint(500, 80),0});          // 修船厂2
    m_slots.append({QPoint(500, 130),0});          // 修船厂3
    m_slots.append({QPoint(50,200),0});          // 海盗1
    m_slots.append({QPoint(100,200),0});               // 海盗2
    m_slots.append({QPoint(520, 330),0});        // 保险
    m_slots.append({QPoint(50, 300),0});        // 领航员1
    m_slots.append({QPoint(50, 350),0})      // 领航员2
    m_slots.append({QPoint(100, 500),1});      // 白船1
    m_slots.append({QPoint(100, 530),1});        // 白船2
    m_slots.append({QPoint(100, 560),1});        // 白船3
    m_slots.append({QPoint(180, 500),2});      // 蓝船1
    m_slots.append({QPoint(180, 530),2});      // 蓝船2
    m_slots.append({QPoint(180, 560),2});      // 蓝船3
    m_slots.append({QPoint(260, 500),3});      // 黑船1
    m_slots.append({QPoint(260, 530),3});      // 黑船2
    m_slots.append({QPoint(260, 560),3});      // 黑船3
    m_slots.append({QPoint(340, 500),4});      // 绿船1
    m_slots.append({QPoint(340, 530),4});      // 绿船2
    m_slots.append({QPoint(340, 560),4});      // 绿船3
    m_slots.append({QPoint(340, 590),4});      // 绿船4
}

void CheckerBoard::initPieces()
{
    m_pieces.clear();

    // 四个玩家初始像素位置（靠近四个角，向内偏移一点）
    QPoint initPos[4] = {
        QPoint(440,  440),    // 左上
        QPoint(440, 470),    // 右上
        QPoint(440, 500),   // 右下
        QPoint(440, 530)    // 左下
    };

    for (int p = 0; p < 4; p++) {
        for (int i = 0; i < 3; i++) {
            Piece piece;
            //QPoint offset;
            // switch (p) {
            // case 0: offset = QPoint( i * 28, 0); break;
            // case 1: offset = QPoint(0, i * 28);  break;
            // case 2: offset = QPoint(-i * 28, 0); break;
            // case 3: offset = QPoint(0, -i * 28); break;
            // }
            piece.pos      = initPos[p];
            piece.player   = p;
            piece.placed   = false;
            piece.dragging = false;
            m_pieces.append(piece);
        }
    }
}



int CheckerBoard::hitSlot(const QPoint &pixel, double radius) const
{
    int best = -1;
    double bestDist = radius;
    for (int i = 0; i < m_slots.size(); i++) {
        double d = std::hypot(m_slots[i].x() - pixel.x(),
                              m_slots[i].y() - pixel.y());
        if (d < bestDist) { bestDist = d; best = i; }
    }
    return best;
}

void CheckerBoard::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(245, 240, 230));

    // 1) 画棋盘连线（用 slot 像素点直接连）
    painter.setPen(QPen(QColor(150, 140, 120), 2));

    // 2) 画所有 slot 圆点
    for (const QPoint &sp : m_slots) {
        bool occupied = false;
        for (const Piece &pc : m_pieces) {
            if (pc.placed && std::hypot(pc.pos.x()-sp.x(), pc.pos.y()-sp.y()) < 4) {
                occupied = true; break;
            }
        }
        painter.setBrush(occupied ? QColor(200,200,200) : Qt::white);
        painter.setPen(QPen(QColor(120,110,100), 1.5));
        painter.drawEllipse(sp, SLOT_RADIUS, SLOT_RADIUS);
    }

    // 3) 画非拖动棋子
    for (const Piece &pc : m_pieces) {
        if (pc.dragging) continue;
        QColor col = PLAYER_COLORS[pc.player];
        QRadialGradient g(pc.pos - QPoint(4,4), PIECE_RADIUS*1.4);
        g.setColorAt(0, col.lighter(160));
        g.setColorAt(1, col);
        painter.setBrush(g);
        painter.setPen(QPen(col.darker(150), 1.5));
        painter.drawEllipse(pc.pos, PIECE_RADIUS, PIECE_RADIUS);

        painter.setBrush(QColor(255,255,255,110));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(pc.pos - QPoint(4,5), 4, 4);
    }

    // 4) 画拖动棋子 + 目标高亮
    if (m_draggingIndex >= 0) {
        const Piece &pc = m_pieces[m_draggingIndex];
        QColor col = PLAYER_COLORS[pc.player];
        painter.setOpacity(0.85);
        QRadialGradient g(pc.pos - QPoint(4,4), PIECE_RADIUS*1.4);
        g.setColorAt(0, col.lighter(160));
        g.setColorAt(1, col);
        painter.setBrush(g);
        painter.setPen(QPen(col.darker(150), 2));
        painter.drawEllipse(pc.pos, PIECE_RADIUS+1, PIECE_RADIUS+1);
        painter.setOpacity(1.0);

        int slot = hitSlot(pc.pos, 40);
        if (slot >= 0) {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(50,50,220), 2.5));
            painter.drawEllipse(m_slots[slot], PIECE_RADIUS+4, PIECE_RADIUS+4);
        }
    }
}

void CheckerBoard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    for (int i = m_pieces.size()-1; i >= 0; i--) {
        Piece &pc = m_pieces[i];
        QPoint d = event->pos() - pc.pos;
        if (std::hypot(d.x(), d.y()) <= PIECE_RADIUS + 4) {
            m_draggingIndex = i;
            m_dragOffset    = d;
            pc.dragging     = true;
            pc.placed       = false;
            update();
            return;
        }
    }
}

void CheckerBoard::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingIndex < 0) return;
    Piece &pc = m_pieces[m_draggingIndex];
    QPoint np = event->pos() - m_dragOffset;
    int r = PIECE_RADIUS;
    np.setX(qBound(r, np.x(), width()-r));
    np.setY(qBound(r, np.y(), height()-r));
    pc.pos = np;
    update();
}

void CheckerBoard::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || m_draggingIndex < 0) return;

    Piece &pc = m_pieces[m_draggingIndex];
    int slot = hitSlot(pc.pos, 40);

    if (slot >= 0) {
        QPoint target = m_slots[slot];
        bool occupied = false;
        for (int j = 0; j < m_pieces.size(); j++) {
            if (j == m_draggingIndex) continue;
            const Piece &o = m_pieces[j];
            if (o.placed && std::hypot(o.pos.x()-target.x(), o.pos.y()-target.y()) < 4) {
                occupied = true; break;
            }
        }
        if (!occupied) {
            pc.pos = target;
            pc.placed = true;
        } else {
            // 弹回初始位置
            QPoint initPos[4] = {        QPoint(440,  440),    // 左上
                                  QPoint(440, 470),    // 右上
                                  QPoint(440, 500),   // 右下
                                  QPoint(440, 530)    // 左下
            };
            pc.pos = initPos[pc.player];
            pc.placed = false;
        }
    } else {
        QPoint initPos[4] = {        QPoint(440,  440),    // 左上
                              QPoint(440, 470),    // 右上
                              QPoint(440, 500),   // 右下
                              QPoint(440, 530)    // 左下
        };
        pc.pos = initPos[pc.player];
        pc.placed = false;
    }

    pc.dragging = false;
    m_draggingIndex = -1;
    update();
}