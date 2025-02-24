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

#ifndef USERACCOUNTSDLG_H
#define USERACCOUNTSDLG_H

#include "useraccountsmodel.h"

#include "ui_useraccounts.h"

enum UserAccountsDisplay
{
    UAD_READWRITE,
    UAD_READONLY,
};

class UserAccountsDlg : public QDialog
{
    Q_OBJECT

public:
    UserAccountsDlg(const useraccounts_t& useraccounts, UserAccountsDisplay uad, QWidget* parent = 0);
    ~UserAccountsDlg();

public:
    void slotCmdSuccess(int cmdid);
    void slotCmdError(int error, int cmdid);

protected:
    void keyPressEvent(QKeyEvent* e) override;

private:
    Ui::UserAccountsDlg ui;
    UserAccountsModel* m_useraccountsModel;
    UserRightsModel* m_userrightsModel;
    QSortFilterProxyModel* m_proxyModel;
    int m_add_cmdid, m_del_cmdid;

    UserAccount m_add_user;
    QString m_del_username;

    AbusePrevention m_abuse;
    void lockUI(bool locked);
    void showUserAccount(const UserAccount& useraccount);
    void updateUserRights(const UserAccount& useraccount);
    void toggleUserRights(const QModelIndex &index);
    UserTypes getUserType();
    UserAccountsDisplay m_uad;

private:
    void slotClearUser();
    void slotAddUser();
    void slotDelUser();
    void slotUserSelected(const QModelIndex & index );
    void slotEdited(const QString&);
    void slotUserTypeChanged();
    void slotCustomCmdLimit(int index);

    void slotAddOpChannel();
    void slotRemoveOpChannel();

    void slotUsernameChanged(const QString& text);
    void slotTreeContextMenu(const QPoint&);
};

#endif
