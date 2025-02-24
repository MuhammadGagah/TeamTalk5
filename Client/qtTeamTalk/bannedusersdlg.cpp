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

#include "bannedusersdlg.h"
#include "appinfo.h"
#include "settings.h"
#include "utilui.h"

#include <QPushButton>

extern QSettings* ttSettings;

enum
{
    COLUMN_INDEX_NICKNAME = 0,
    COLUMN_INDEX_USERNAME,
    COLUMN_INDEX_BANTYPE,
    COLUMN_INDEX_BANTIME,
    COLUMN_INDEX_CHANPATH,
    COLUMN_INDEX_IPADDRESS,
    COLUMN_COUNT_BANNEDUSERS
};

extern TTInstance* ttInst;

BannedUsersModel::BannedUsersModel(QObject* parent)
: QAbstractItemModel(parent)
{
}

QVariant BannedUsersModel::headerData ( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */) const
{
    switch(role)
    {
    case Qt::DisplayRole :
        if(orientation == Qt::Horizontal)
            switch(section)
        {
            case COLUMN_INDEX_NICKNAME: return tr("Nickname");
            case COLUMN_INDEX_USERNAME: return tr("Username");
            case COLUMN_INDEX_BANTYPE: return tr("Ban type");
            case COLUMN_INDEX_BANTIME: return tr("Ban Time");
            case COLUMN_INDEX_CHANPATH: return tr("Channel");
            case COLUMN_INDEX_IPADDRESS: return tr("IP-address");
        }
    }
    return QVariant();
}

int BannedUsersModel::columnCount ( const QModelIndex & parent /*= QModelIndex() */) const
{
    return COLUMN_COUNT_BANNEDUSERS;
}

QVariant BannedUsersModel::data ( const QModelIndex & index, int role /*= Qt::DisplayRole */) const
{
    Q_ASSERT(index.isValid());
    switch(role)
    {
    case Qt::DisplayRole :
        Q_ASSERT(index.row() < m_users.size());
        switch(index.column())
        {
        case COLUMN_INDEX_NICKNAME :
            return _Q(m_users[index.row()].szNickname);
        case COLUMN_INDEX_USERNAME :
            return _Q(m_users[index.row()].szUsername);
        case COLUMN_INDEX_BANTYPE :
        {
            QString bantype;
            if (m_users[index.row()].uBanTypes & BANTYPE_USERNAME)
                bantype += tr("User");
            if (m_users[index.row()].uBanTypes & BANTYPE_IPADDR)
                bantype += (bantype.size()? tr(",IP") : tr("IP"));
            if (m_users[index.row()].uBanTypes & BANTYPE_CHANNEL)
                bantype += (bantype.size()? tr(",Channel") : tr("Channel"));
            return bantype;
        }
        case COLUMN_INDEX_BANTIME :
            return _Q(m_users[index.row()].szBanTime);
        case COLUMN_INDEX_CHANPATH :
            return _Q(m_users[index.row()].szChannelPath);
        case COLUMN_INDEX_IPADDRESS : 
            return _Q(m_users[index.row()].szIPAddress);
        }
        break;
        case Qt::AccessibleTextRole :
        {
            return QString("%1: %2, %3: %4, %5: %6, %7: %8, %9: %10, %11: %12").arg(headerData(COLUMN_INDEX_NICKNAME, Qt::Horizontal, Qt::DisplayRole).toString()).arg(data(createIndex(index.row(), COLUMN_INDEX_NICKNAME, index.internalId()), Qt::DisplayRole).toString()).arg(headerData(COLUMN_INDEX_USERNAME, Qt::Horizontal, Qt::DisplayRole).toString()).arg(data(createIndex(index.row(), COLUMN_INDEX_USERNAME, index.internalId()), Qt::DisplayRole).toString()).arg(headerData(COLUMN_INDEX_BANTYPE, Qt::Horizontal, Qt::DisplayRole).toString()).arg(data(createIndex(index.row(), COLUMN_INDEX_BANTYPE, index.internalId()), Qt::DisplayRole).toString()).arg(headerData(COLUMN_INDEX_BANTIME, Qt::Horizontal, Qt::DisplayRole).toString()).arg(data(createIndex(index.row(), COLUMN_INDEX_BANTIME, index.internalId()), Qt::DisplayRole).toString()).arg(headerData(COLUMN_INDEX_CHANPATH, Qt::Horizontal, Qt::DisplayRole).toString()).arg(data(createIndex(index.row(), COLUMN_INDEX_CHANPATH, index.internalId()), Qt::DisplayRole).toString()).arg(headerData(COLUMN_INDEX_IPADDRESS, Qt::Horizontal, Qt::DisplayRole).toString()).arg(data(createIndex(index.row(), COLUMN_INDEX_IPADDRESS, index.internalId()), Qt::DisplayRole).toString());
        }
    }
    return QVariant();
}

QModelIndex BannedUsersModel::index ( int row, int column, const QModelIndex & parent /*= QModelIndex()*/ ) const
{
    if(!parent.isValid() && row<m_users.size())
        return createIndex(row, column);
    return QModelIndex();
}

QModelIndex BannedUsersModel::parent ( const QModelIndex &/* index */) const
{
    return QModelIndex();
}

int BannedUsersModel::rowCount ( const QModelIndex & /*parent = QModelIndex() */) const
{
    return m_users.size();
}

void BannedUsersModel::addBannedUser(const BannedUser& user, bool do_reset)
{
    m_users.push_back(user);
    if(do_reset)
    {
        this->beginResetModel();
        this->endResetModel();
    }
}

void BannedUsersModel::delBannedUser(int index)
{
    if(m_users.size())
        m_users.erase(m_users.begin()+index);

    this->beginResetModel();
    this->endResetModel();
}

BannedUsersDlg::BannedUsersDlg(const bannedusers_t& bannedusers, const QString& chanpath, QWidget * parent/* = 0*/)
    : QDialog(parent, QT_DEFAULT_DIALOG_HINTS | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint)
    , m_chanpath(chanpath)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(APPICON));
    restoreGeometry(ttSettings->value(SETTINGS_DISPLAY_BANNEDUSERSWINDOWPOS).toByteArray());

    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&OK"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("&Cancel"));

    m_bannedmodel = new BannedUsersModel(this);
    m_bannedproxy = new QSortFilterProxyModel(this);
    m_bannedproxy->setSourceModel(m_bannedmodel);
    m_unbannedmodel = new BannedUsersModel(this);
    m_unbannedproxy = new QSortFilterProxyModel(this);
    m_unbannedproxy->setSourceModel(m_unbannedmodel);

    for(int i=0;i<bannedusers.size();i++)
        m_bannedmodel->addBannedUser(bannedusers[i], i+1 == bannedusers.size());

#if defined(Q_OS_MAC)
    auto font = ui.bannedTreeView->font();
    font.setPointSize(13);
    ui.bannedTreeView->setFont(font);
    ui.unbannedTreeView->setFont(font);
#endif

    ui.bannedTreeView->setModel(m_bannedproxy);
    for(int i=0;i<COLUMN_COUNT_BANNEDUSERS;i++)
        ui.bannedTreeView->resizeColumnToContents(i);
    ui.unbannedTreeView->setModel(m_unbannedproxy);
    for(int i=0;i<COLUMN_COUNT_BANNEDUSERS;i++)
        ui.unbannedTreeView->resizeColumnToContents(i);

    auto banfunc = [&]() {
        ui.newbanBtn->setEnabled( (getCurrentItemData(ui.bantypeBox).toInt() & BANTYPE_IPADDR) == BANTYPE_NONE || ui.banEdit->text().size());
    };
    connect(ui.newbanBtn, &QAbstractButton::clicked, this, &BannedUsersDlg::slotNewBan);
    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &BannedUsersDlg::slotClose);
    connect(ui.leftButton, &QAbstractButton::clicked, this, &BannedUsersDlg::slotBanUser);
    connect(ui.rightButton, &QAbstractButton::clicked, this, &BannedUsersDlg::slotUnbanUser);
    connect(ui.bannedTreeView, &QTreeView::doubleClicked, this, &BannedUsersDlg::slotUnbanUser);
    connect(ui.unbannedTreeView, &QTreeView::doubleClicked, this, &BannedUsersDlg::slotBanUser);
    connect(ui.banEdit, &QLineEdit::textEdited, banfunc);
    connect(ui.bantypeBox, &QComboBox::currentTextChanged, banfunc);

    ui.bantypeBox->addItem(tr("Ban IP-address"), BanTypes(BANTYPE_IPADDR));
    ui.bantypeBox->addItem(tr("Ban Username"), BanTypes(BANTYPE_USERNAME));
}

BannedUsersDlg::~BannedUsersDlg()
{
    ttSettings->setValue(SETTINGS_DISPLAY_BANNEDUSERSWINDOWPOS, saveGeometry());
}

void BannedUsersDlg::slotClose()
{
    bannedusers_t users = m_unbannedmodel->getUsers();
    for(int i=0;i<users.size();i++)
        TT_DoUnBanUserEx(ttInst, &users[i]);
}

void BannedUsersDlg::slotUnbanUser()
{
    int index = m_bannedproxy->mapToSource(ui.bannedTreeView->currentIndex()).row();
    if(index<0)
        return;
    m_unbannedmodel->addBannedUser(m_bannedmodel->getUsers()[index], true);
    m_bannedmodel->delBannedUser(index);
}

void BannedUsersDlg::slotBanUser()
{
    int index = m_unbannedproxy->mapToSource(ui.unbannedTreeView->currentIndex()).row();
    if(index<0)
        return;
    m_bannedmodel->addBannedUser(m_unbannedmodel->getUsers()[index], true);
    m_unbannedmodel->delBannedUser(index);
}

void BannedUsersDlg::slotNewBan()
{
    BannedUser ban = {};
    ban.uBanTypes = BanTypes(ui.bantypeBox->currentData().toInt());
    if (m_chanpath.size())
    {
        ban.uBanTypes |= BANTYPE_CHANNEL;
        COPY_TTSTR(ban.szChannelPath, m_chanpath);
    }
    if (ui.bantypeBox->currentData().toInt() & BANTYPE_IPADDR)
        COPY_TTSTR(ban.szIPAddress, ui.banEdit->text());
    if (ui.bantypeBox->currentData().toInt() & BANTYPE_USERNAME)
        COPY_TTSTR(ban.szUsername, ui.banEdit->text());
    ui.banEdit->setText("");

    if(TT_DoBan(ttInst, &ban) > 0)
    {
        m_bannedmodel->addBannedUser(ban, true);
    }
}
