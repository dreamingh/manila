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
    setFixedSize(800,800);

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
    m_slots.append({QPoint(50, 350),0});      // 领航员2
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

    for (int p = 0; p < 4; p++)
    {
        for (int i = 0; i < 3; i++)
        {
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
    for (int i = 0; i < m_slots.size(); i++)
    {
        double d = std::hypot(m_slots[i].pos.x() - pixel.x(),
                              m_slots[i].pos.y() - pixel.y());
        if (d < bestDist)
        {
            bestDist = d; best = i;
        }
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
    // 1) 画 13 个 plot
    for (int i = 0; i < m_slots.size(); i++) {
        const Slot &s = m_slots[i];

        // 该 plot 是否已被占用
        bool occupied = false;
        for (const Piece &pc : m_pieces) {
            if (pc.placed &&
                std::hypot(pc.pos.x() - s.pos.x(), pc.pos.y() - s.pos.y()) < 4) {
                occupied = true;
                break;
            }
        }

        QColor fill, border;
        if (m_moduleDisabled[s.module])
        {
            // 模块被禁用 → 灰色 + 斜线感（用深灰）
            fill   = QColor(160, 160, 160);
            border = QColor(100, 100, 100);
        }
        else if (occupied)
        {
            fill   = QColor(200, 200, 200);
            border = QColor(120, 110, 100);
        }
        else
        {
            fill   = Qt::white;
            border = QColor(120, 110, 100);
        }

        painter.setBrush(fill);
        painter.setPen(QPen(border, 1.5));
        painter.drawEllipse(s.pos, SLOT_RADIUS, SLOT_RADIUS);

        // 禁用的 plot 额外画一个小叉（用两条短线表示，不用 drawLine 的话可以用矩形）
        if (m_moduleDisabled[s.module])
        {
            painter.setPen(QPen(QColor(80, 80, 80), 1.2));
            int r = SLOT_RADIUS - 2;
            painter.drawRect(s.pos.x() - r, s.pos.y() - r, r * 2, r * 2);
        }
    }

    // 3) 画非拖动棋子
    for (const Piece &pc : m_pieces)
    {
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
        if (slot >= 0)
        {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(50,50,220), 2.5));
            painter.drawEllipse(m_slots[slot].pos, PIECE_RADIUS+4, PIECE_RADIUS+4);
        }
    }

    // ===== paintEvent 末尾：拖动中的棋子 + 目标高亮 =====
    if (m_draggingIndex >= 0) {
        const Piece &pc = m_pieces[m_draggingIndex];   // ← pc 在这里定义

        // 1) 画拖动中的棋子
        QColor col = PLAYER_COLORS[pc.player];
        painter.setOpacity(0.85);
        QRadialGradient g(pc.pos - QPoint(4, 4), PIECE_RADIUS * 1.4);
        g.setColorAt(0, col.lighter(160));
        g.setColorAt(1, col);
        painter.setBrush(g);
        painter.setPen(QPen(col.darker(150), 2));
        painter.drawEllipse(pc.pos, PIECE_RADIUS + 1, PIECE_RADIUS + 1);
        painter.setOpacity(1.0);

        // 2) 目标 slot 高亮（含禁用模块判断）
        int slot = hitSlot(pc.pos, 40);
        if (slot >= 0) {
            const Slot &s = m_slots[slot];
            QColor highlight = m_moduleDisabled[s.module]
                                   ? QColor(200, 50, 50)     // 红：禁止
                                   : QColor(50, 50, 220);    // 蓝：可放置
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(highlight, 2.5));
            painter.drawEllipse(s.pos, PIECE_RADIUS + 4, PIECE_RADIUS + 4);
        }
    }
}

void CheckerBoard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    for (int i = m_pieces.size()-1; i >= 0; i--)
    {
        Piece &pc = m_pieces[i];
        QPoint d = event->pos() - pc.pos;
        if (std::hypot(d.x(), d.y()) <= PIECE_RADIUS + 4)
        {
            m_draggingIndex = i;
            m_dragOffset    = d;
            pc.dragging     = true;
            pc.placed       = false;
            update();
            return;
        }
    }
    // 2) 没点中棋子 → 判断是否点中某个模块区域
    int module = hitModule(event->pos());
    if (module >= 0) {
        // 切换禁用状态（再点一次可恢复，按你的需求决定是否允许恢复）
        m_moduleDisabled[module] = true;
        update();
        return;
    }

    // 3) 点空白，什么都不做
    m_draggingIndex = -1;
    m_dragOffset    = QPoint(0, 0);
}
QRect CheckerBoard::moduleRect(int module){
    switch (module) {
    case 0: return QRect(80,  80,  240, 100);   // 左上
    case 1: return QRect(360, 80,  240, 100);   // 右上
    case 2: return QRect(80,  480, 240, 100);   // 左下
    case 3: return QRect(340, 480, 260, 100);   // 右下
    }
    return QRect();
}

int CheckerBoard::hitModule(const QPoint &p)
{
    for (int m = 0; m < 4; m++) {
        if (moduleRect(m).contains(p))
            return m;
    }
    return -1;
}

void CheckerBoard::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingIndex < 0) return;

    Piece &pc = m_pieces[m_draggingIndex];   // 这里才有 pc
    QPoint np = event->pos() - m_dragOffset;

    // 限制在窗口内
    int r = PIECE_RADIUS;
    np.setX(qBound(r, np.x(), width()  - r));
    np.setY(qBound(r, np.y(), height() - r));

    pc.pos = np;
    update();
}
void CheckerBoard::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || m_draggingIndex < 0) return;

    Piece &pc = m_pieces[m_draggingIndex];
    int slot = hitSlot(pc.pos, 40);

    if (slot >= 0) {
        const Slot &s = m_slots[slot];

        // ★ 关键：模块被禁用 → 不能放入
        if (m_moduleDisabled[s.module]) {
            // 弹回初始位置
            QPoint initPos[4] = {QPoint(40,40), QPoint(600,40), QPoint(600,600), QPoint(40,600)};
            pc.pos    = initPos[pc.player];
            pc.placed = false;
            pc.dragging = false;
            m_draggingIndex = -1;
            update();
            return;
        }

        // 检查该 plot 是否已被占用
        QPoint target = s.pos;
        bool occupied = false;
        for (int j = 0; j < m_pieces.size(); j++) {
            if (j == m_draggingIndex) continue;
            const Piece &o = m_pieces[j];
            if (o.placed &&
                std::hypot(o.pos.x() - target.x(), o.pos.y() - target.y()) < 4) {
                occupied = true;
                break;
            }
        }

        if (!occupied) {
            pc.pos    = target;
            pc.placed = true;
        } else {
            QPoint initPos[4] = {QPoint(40,40), QPoint(600,40), QPoint(600,600), QPoint(40,600)};
            pc.pos    = initPos[pc.player];
            pc.placed = false;
        }
    } else {
        QPoint initPos[4] = {QPoint(40,40), QPoint(600,40), QPoint(600,600), QPoint(40,600)};
        pc.pos    = initPos[pc.player];
        pc.placed = false;
    }

    pc.dragging     = false;
    m_draggingIndex = -1;
    update();

}