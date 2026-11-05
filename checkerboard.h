#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QMouseEvent>
#include <QPaintEvent>

struct Piece {
    QPoint pos;      // 像素位置
    QPointF gridPos; // 网格坐标（用于判断落在哪个格子）
    int player;      // 0-3 表示四方，-1 表示未放置
    bool placed;     // 是否已放置到格子上
    bool dragging;   // 是否正在拖动
};

class CheckerBoard : public QWidget
{
    Q_OBJECT

public:
    explicit CheckerBoard(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initSlots();       // 用像素坐标定义 24 个 slot
    void initPieces();      // 棋子初始像素位置

    int hitSlot(const QPoint &pixel, double radius) const; // 最近 slot

    // 棋子半径（调小，配合更细的格子）
    static constexpr int PIECE_RADIUS = 13;
    static constexpr int SLOT_RADIUS  = 6;   // 格子圆点半径

    QVector<QPoint>   m_slots;      // 24 个 slot 的【像素坐标】
    QVector<Piece>    m_pieces;

    int    m_draggingIndex = -1;
    QPoint m_dragOffset;
};

#endif // CHECKERBOARD_H