/**
 *	AutoStruct
 *
 *	Copyright (C) 2015  Christos Korras
 *
 *	This program is proprietary software: you cannot redistribute it and/or modify it.
 *
 *	This program is distributed WITHOUT ANY WARRANTY; without even the implied warranty of 
 *	FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	You should have received a copy of the Licenset along with this program.
 *
 *	If not, see <http://www.autostruct.com/licenses/>.
 *
**/

#include "filescenetransitions_tosavefiletransition.h"

#include <events/include/filesceneevents_tosavefileevent.h>
#include <events/include/registerevents_userevents.h>
#include <graphics/include/drawitems_ortho.h>
#include <graphics/include/drawitems_beam.h>
#include <graphics/include/drawitems_boundaryline.h>
#include <graphics/include/drawitems_wall.h>
#include <graphics/include/drawitems_foot.h>
#include <graphics/include/drawitems_strip.h>
#include <views/include/elev_drawingboard.h>
#include <models/include/world_drawingboard.h>
#include <models/include/dbproxy_ortho.h>
#include <models/include/dbproxy_beam.h>
#include <models/include/dbproxy_boundaryline.h>
#include <models/include/dbproxy_wall.h>
#include <models/include/dbproxy_foot.h>
#include <models/include/dbproxy_strip.h>
#include <db/include/vars_tinstance.h>
#include <specs/include/statesceneinterface_drawingstate.h>
#include <ui/include/windows_mainwindow.h>
#include <projects/include/building_levels.h>
#include <projects/include/building_level.h>
#include <tools/include/logger_categories.h>

#include <QtGlobal>
#include <QtDebug>
#include <QState>
#include <QStateMachine>
#include <QEvent>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QDataStream>
#include <QByteArray>
#include <QIODevice>
#include <QGraphicsItem>

#include <memory>

namespace RegisterEvents = events::registerevents;
namespace Specs = specs::statesceneinterface;
namespace Views = views::elev;
namespace Models = models::world;
namespace FileSceneEvents = events::filesceneevents;
namespace Ui = ui::windows;

	namespace /* unit */	{

	/*********************************************
	*
	* constants
	*
	**********************************************/


	/*********************************************
	*
	* types
	*
	**********************************************/

	}	// namespace unit

	namespace transitions		{
		namespace filescenetransitions		{

	/////////////helper classes//////////////////

	/*********************************************
	*
	* interface filescenetransitions_ToSaveFileTransitionImpl01
	*
	**********************************************/

	class filescenetransitions_ToSaveFileTransition::filescenetransitions_ToSaveFileTransitionImpl01		{
		// data
		private:
			filescenetransitions_ToSaveFileTransition* m_pToSaveFileTransition;

            Ui::windows_MainWindow* m_pMainWindow;
	
		// constructors
		public:
			filescenetransitions_ToSaveFileTransitionImpl01() = default;
			filescenetransitions_ToSaveFileTransitionImpl01(
                filescenetransitions_ToSaveFileTransition*,
                Specs::statesceneinterface_DrawingState* ai_pTargetState,
                Ui::windows_MainWindow* ai_pMainWindow
            );
			filescenetransitions_ToSaveFileTransitionImpl01(filescenetransitions_ToSaveFileTransitionImpl01 const&) = default;
			filescenetransitions_ToSaveFileTransitionImpl01(filescenetransitions_ToSaveFileTransitionImpl01&&) = default;
			~filescenetransitions_ToSaveFileTransitionImpl01() = default;


		// operators
		public:
			filescenetransitions_ToSaveFileTransitionImpl01& operator = (filescenetransitions_ToSaveFileTransitionImpl01 const&) = default;
			filescenetransitions_ToSaveFileTransitionImpl01& operator = (filescenetransitions_ToSaveFileTransitionImpl01&&) = default;

        // operations
	    public:	
            bool mc_bEventTest(QEvent* ai_pEvent)   const;
            void m_vOnTransition(QEvent* ai_pEvent);

	};

	/*********************************************
	*
	* implementation filescenetransitions_ToSaveFileTransitionImpl01	
	*
	**********************************************/
	// constructors
	filescenetransitions_ToSaveFileTransition::filescenetransitions_ToSaveFileTransitionImpl01::filescenetransitions_ToSaveFileTransitionImpl01(
        filescenetransitions_ToSaveFileTransition* ai_pToSaveFileTransition,
        Specs::statesceneinterface_DrawingState* ai_pTargetState,
        Ui::windows_MainWindow* ai_pMainWindow
    )
		:	m_pToSaveFileTransition(ai_pToSaveFileTransition),
            m_pMainWindow(ai_pMainWindow)
	{
		Q_ASSERT(m_pToSaveFileTransition);
        Q_ASSERT(m_pMainWindow);
        Q_ASSERT(ai_pTargetState);

        m_pToSaveFileTransition->setTargetState(ai_pTargetState);
	}

    // operations
    bool filescenetransitions_ToSaveFileTransition::filescenetransitions_ToSaveFileTransitionImpl01::mc_bEventTest(QEvent* ai_pEvent)   const   {
        Q_ASSERT(ai_pEvent);
        
        if (static_cast<int>(RegisterEvents::registerevents_UserEvents::TO_SAVE_FILE_SCENE_EVENT) != ai_pEvent->type()) 
            return false
        ;

        return true;
    }

    void filescenetransitions_ToSaveFileTransition::filescenetransitions_ToSaveFileTransitionImpl01::m_vOnTransition(QEvent* ai_pEvent) {
        Q_ASSERT(m_pToSaveFileTransition);
        Q_ASSERT(m_pMainWindow);
        Q_ASSERT(m_pMainWindow->mc_pGetStateMachine());
        Q_ASSERT(ai_pEvent);

        qDebug(logDebug).noquote()
            <<  "inside filescenetransitions_ToSaveFileTransition::filescenetransitions_ToSaveFileTransitionImpl01::m_vOnTransition()"
        ;

        FileSceneEvents::filesceneevents_ToSaveFileEvent* l_pSaveFileSceneEvent =
            static_cast<FileSceneEvents::filesceneevents_ToSaveFileEvent*>(ai_pEvent)
        ;
        Q_ASSERT(l_pSaveFileSceneEvent);
        
       auto l_spInstance = m_pMainWindow->mvc_qspGetCurrentProjectInstance();
       Q_ASSERT(l_spInstance);

        QStringList l_qslSaveFloors = l_pSaveFileSceneEvent->mc_qslGetLevels();
        for (auto l_qsSaveFloor : l_qslSaveFloors)  {
            QStringList l_qslSaveFloorPath = l_qsSaveFloor.split('/');  
            Q_ASSERT(!l_qslSaveFloorPath.empty());

            QString l_qsStathmi = l_qslSaveFloorPath.takeLast();
            QString l_qsSaveProject = l_qslSaveFloorPath.join('/');

            QMdiArea* l_pqMdiArea = qobject_cast<QMdiArea*>(m_pMainWindow->centralWidget());
            Q_ASSERT(l_pqMdiArea);


            Models::world_DrawingBoard* l_pWorldDrawingBoard = nullptr; 
            auto l_qlpObjects = l_pqMdiArea->children();
            for (auto l_pqObject : l_qlpObjects)    {
                l_pWorldDrawingBoard = qobject_cast<Models::world_DrawingBoard*>(l_pqObject);    
                if (
                        l_pWorldDrawingBoard
                        &&
                        l_qsSaveProject == l_pWorldDrawingBoard->mc_qsGetProject()
                        &&
                        l_qsStathmi == l_pWorldDrawingBoard->mc_qsGetStathmi()
                ) 
                    break
                ;
                else
                    l_pWorldDrawingBoard = nullptr
                ;
            }
            Q_ASSERT(l_pWorldDrawingBoard);

            if (l_pWorldDrawingBoard->mc_bHasSceneChanged()) {
                QVector<std::shared_ptr<models::dbproxy::dbproxy_Ortho>> l_qvecpDbProxyOrthos;
                QVector<std::shared_ptr<models::dbproxy::dbproxy_Beam>> l_qvecpDbProxyBeams;
                QVector<std::shared_ptr<models::dbproxy::dbproxy_BoundaryLine>> l_qvecpDbProxyBoundaryLines;
                QVector<std::shared_ptr<models::dbproxy::dbproxy_Wall>> l_qvecpDbProxyWalls;
                QVector<std::shared_ptr<models::dbproxy::dbproxy_Foot>> l_qvecpDbProxyFoots;
                QVector<std::shared_ptr<models::dbproxy::dbproxy_Strip>> l_qvecpDbProxyStrips;

                auto l_qlpGraphicsItems = l_pWorldDrawingBoard->items();
                for (auto l_qpGraphicsItem : l_qlpGraphicsItems)    {
                    if (dynamic_cast<graphics::drawitems::drawitems_Ortho*>(l_qpGraphicsItem))    {
                        l_qvecpDbProxyOrthos.push_back(dynamic_cast<graphics::drawitems::drawitems_Ortho*>(l_qpGraphicsItem)->mc_spGetDbProxy());
                    }
                    else if (dynamic_cast<graphics::drawitems::drawitems_Beam*>(l_qpGraphicsItem))    {
                        l_qvecpDbProxyBeams.push_back(dynamic_cast<graphics::drawitems::drawitems_Beam*>(l_qpGraphicsItem)->mc_spGetDbProxy());
                    }
                    else if (dynamic_cast<graphics::drawitems::drawitems_BoundaryLine*>(l_qpGraphicsItem))    {
                        l_qvecpDbProxyBoundaryLines.push_back(dynamic_cast<graphics::drawitems::drawitems_BoundaryLine*>(l_qpGraphicsItem)->mc_spGetDbProxy());
                    }
                    else if (dynamic_cast<graphics::drawitems::drawitems_Wall*>(l_qpGraphicsItem))    {
                        l_qvecpDbProxyWalls.push_back(dynamic_cast<graphics::drawitems::drawitems_Wall*>(l_qpGraphicsItem)->mc_spGetDbProxy());
                    }
                    else if (dynamic_cast<graphics::drawitems::drawitems_Foot*>(l_qpGraphicsItem))    {
                        l_qvecpDbProxyFoots.push_back(dynamic_cast<graphics::drawitems::drawitems_Foot*>(l_qpGraphicsItem)->mc_spGetDbProxy()); }
                    else if (dynamic_cast<graphics::drawitems::drawitems_Strip*>(l_qpGraphicsItem))    {
                        l_qvecpDbProxyStrips.push_back(dynamic_cast<graphics::drawitems::drawitems_Strip*>(l_qpGraphicsItem)->mc_spGetDbProxy());
                    }
                }

                auto l_spqDbProxiesByteArray = std::make_shared<QByteArray>();
                QDataStream l_qdsDbProxiesDataStream(l_spqDbProxiesByteArray.get(), QIODevice::WriteOnly);
                l_qdsDbProxiesDataStream.setVersion(QDataStream::Qt_5_15);
                l_qdsDbProxiesDataStream
                    <<  l_pWorldDrawingBoard->mc_GetLevel()
                    <<  l_qvecpDbProxyOrthos
                    <<  l_qvecpDbProxyBeams
                    <<  l_qvecpDbProxyBoundaryLines
                    <<  l_qvecpDbProxyWalls
                    <<  l_qvecpDbProxyFoots
                    <<  l_qvecpDbProxyStrips
                ;

                auto l_qeStatus = l_qdsDbProxiesDataStream.status();
                qDebug(logDebug).noquote()
                    <<  "the data stream status after the elements insertion is: " + QString::number(l_qeStatus)
                ;

                l_spInstance->m_vSave(l_spqDbProxiesByteArray, l_qsSaveFloor);

                l_pWorldDrawingBoard->m_vSetSceneChanged(false);
            }
        }

        l_spInstance->m_vSaveLevels();

        m_pMainWindow->m_vSetProjectChanged(false);
    }

/////////////filescenetransitions_ToSaveFileTransition/////////////

// constructors
filescenetransitions_ToSaveFileTransition::filescenetransitions_ToSaveFileTransition(
    QState* ai_pSourceState,
    Specs::statesceneinterface_DrawingState* ai_pTargetState,
    Ui::windows_MainWindow* ai_pMainWindow
)
	:	QAbstractTransition(ai_pSourceState)
{
	Q_ASSERT(!m_spToSaveFileTransitionImpl);
    Q_ASSERT(ai_pSourceState);
    Q_ASSERT(ai_pTargetState);
    Q_ASSERT(ai_pMainWindow);

	m_spToSaveFileTransitionImpl.reset(
		new filescenetransitions_ToSaveFileTransitionImpl01(
            this,
            ai_pTargetState,
            ai_pMainWindow
        )
	);

}

filescenetransitions_ToSaveFileTransition::~filescenetransitions_ToSaveFileTransition()	{}

// overriding functionalities
bool filescenetransitions_ToSaveFileTransition::eventTest(QEvent *ai_pEvent)   {
	Q_ASSERT(m_spToSaveFileTransitionImpl);
    Q_ASSERT(ai_pEvent);

    return m_spToSaveFileTransitionImpl->mc_bEventTest(ai_pEvent);
}

void filescenetransitions_ToSaveFileTransition::onTransition(QEvent *ai_pEvent)   {
	Q_ASSERT(m_spToSaveFileTransitionImpl);
    Q_ASSERT(ai_pEvent);

    m_spToSaveFileTransitionImpl->m_vOnTransition(ai_pEvent);
}

	}	// namespace filescenetransitions
}	// namespace transitions
