/*
 * Copyright (c) 2005-2018, BearWare.dk
 * 
 * Contact Information:
 *
 * Bjoern D. Rasmussen
 * Kirketoften 5
 * DK-8260 Viby J
 * Denmark
 * Email: contact@bearware.dk
 * Phone: +45 20 20 54 59
 * Web: http://www.bearware.dk
 *
 * This source code is part of the TeamTalk SDK owned by
 * BearWare.dk. Use of this file, or its compiled unit, requires a
 * TeamTalk SDK License Key issued by BearWare.dk.
 *
 * The TeamTalk SDK License Agreement along with its Terms and
 * Conditions are outlined in the file License.txt included with the
 * TeamTalk SDK distribution.
 *
 */

#ifndef USERDESKTOPWIDGET_H
#define USERDESKTOPWIDGET_H

#include "userimagewidget.h"
#include "common.h"

class UserDesktopWidget : public UserImageWidget
{
    Q_OBJECT

public:
    UserDesktopWidget(QWidget* parent, int userid = 0, int border_width = 0);
    ~UserDesktopWidget();

    QSize imageSize() const override;

    void paintEvent(QPaintEvent *p) override;

    void setUserID(int userid) override;

public:
    void slotDesktopUpdate(int userid, int sessionid);
    void slotDesktopCursorUpdate(int src_userid, const DesktopInput& input);
    void slotUserUpdated(const User& user);

signals:
    void userDesktopWindowEnded(int userid);

protected:
    void slotContextMenu(const QPoint& p) override;

protected:
    /* Qt inherit */
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    
    //TT send mouse event as desktop input
    void sendMouseEvent(const QPoint& p, quint32 keycode,
                        DesktopKeyState keystate);

    /* Qt inherit */
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    //TT send key event as desktop input
    void sendKeyEvent(quint32 keycode, DesktopKeyState keystate);

    void translateSend(const DesktopInput& deskinput);

#ifdef USE_TT_PAINT
    void runTTPaint(QPainter& painter);
#endif
    void timerEvent(QTimerEvent *e) override;
    void refreshTimeout();
    void sendDesktopInputTimeout();

    int m_refresh_timerid;
    bool m_desktop_updated;
    QPoint m_cursorpos; //shared desktop cursor

    bool m_access_requested;

    bool m_mousedown;
    QPoint m_old_pos;
    //Desktop window to display
    DesktopWindow* m_desktop_window;
    //transmission queue for desktop input
    QVector<DesktopInput> m_sendinput_queue;
    int m_sendinput_timerid;
    //cache of user data
    User m_user;
};

#endif
