/**
 * TeamSpeak 3 Client Notification Plugin
 *
 * Copyright (c) Sven Paulsen. All rights reserved.
 */

#ifndef WIDGETHELPER_H
#define WIDGETHELPER_H

#include "shared.h"
#include "singleton.h"

class WidgetHelper : public QObject, public Singleton<WidgetHelper>
{
    Q_OBJECT

    friend class Singleton<WidgetHelper>;

    QMainWindow*    m_mainWindow  = nullptr;
    QTabWidget*     m_serverTabs  = nullptr;
    QStackedWidget* m_serverStack = nullptr;
    QWidget*        m_serverArea  = nullptr;
    QTabWidget*     m_chatTabs    = nullptr;
    QStackedWidget* m_chatStack   = nullptr;
    QWidget*        m_chatArea    = nullptr;
    QSplitter*      m_splitter    = nullptr;

    bool         m_docked = false;
    bool         m_hooked = false;
    bool         m_loaded = false;
    unsigned int m_hooks  = 0;

    QDockWidget* m_dockInfo;
    QDockWidget* m_dockChat;

    QMap<QWidget*, int> m_widgets;
    QStackedWidget*     m_stacked;

    WidgetHelper();
    ~WidgetHelper();

    template<typename T>
    T getWidget(const char* name, QWidget* parent)
    {
        QList<T> widgets = parent->findChildren<T>();

        for (int i = 0; i < widgets.size(); ++i)
        {
            if(widgets.at(i)->inherits(name) || widgets.at(i)->metaObject()->className() == QString(name) || widgets.at(i)->objectName() == name)
            {
                return (T) widgets.at(i);
            }
        }

        return nullptr;
    }

public:
    unsigned int hook();
    unsigned int unhook();
    unsigned int start();
    unsigned int stop();
    unsigned int backup();
    unsigned int restore();

public slots:
    void onPluginHookFailed();
    void onServerTabChanged(int index);
    void onServerTabClicked(int index);
    void onServerTabDoubleClicked(int index);
    void onServerTabDestroyed(QObject* obj);
    void onChatTabChanged(int index);
    void onChatTabClicked(int index);
    void onChatTabDoubleClicked(int index);
};

#endif // WIDGETHELPER_H
