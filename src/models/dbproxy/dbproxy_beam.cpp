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

#include "dbproxy_beam.h"

#include <db/include/vars_tinstance.h>
#include <db/include/beams_dokos.h>

#include <QString>
#include <QtGlobal>
#include <QtDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QPointF>
#include <QMap>
#include <QDataStream>

	namespace /* unit */	{
	/*********************************************
	*
	* types
	*
	**********************************************/

	/*********************************************
	*
	* constants
	*
	**********************************************/


	/*********************************************
	*
	* dbproxy
	*
	**********************************************/

	}	// namespace unit

	namespace models		{
		namespace dbproxy		{

	/////////////helper classes//////////////////

	/*********************************************
	*
	* interface dbproxy_BeamImpl01
	*
	**********************************************/

	class dbproxy_Beam::dbproxy_BeamImpl01		{
		// data
		private:
			dbproxy_Beam* m_pBeam;

            QSharedPointer<db::vars::vars_TInstance> m_qspInstance;

            QSharedPointer<db::beams::beams_Beam> m_qspBeam;

		// constructors
		public:
			dbproxy_BeamImpl01() = default;
			dbproxy_BeamImpl01(
                dbproxy_Beam*,
                QString const& airc_qsStathmi,
                QSharedPointer<db::vars::vars_TInstance> ai_qspInstance
            );
			dbproxy_BeamImpl01(dbproxy_BeamImpl01 const&) = default;
			dbproxy_BeamImpl01(dbproxy_BeamImpl01&&) = default;
			~dbproxy_BeamImpl01() = default;


		// operators
		public:
			dbproxy_BeamImpl01& operator = (dbproxy_BeamImpl01 const&) = default;
			dbproxy_BeamImpl01& operator = (dbproxy_BeamImpl01&&) = default;

        // accessors
        public:
            QUuid mc_quuidGetId()   const;
            QString mc_qsGetLabel() const;
            db::beams::beams_Beam::Position::type mc_tupleGetPosition()    const;
            db::beams::beams_Beam::Dimensions::type mc_tupleGetDimensions()    const;
            db::beams::beams_Beam::Beam::type mc_tupleGetBeam()   const;

        // modifiers
        public:
            void m_vSetId(QUuid const& airc_quuidId);
            void m_vSetLabel(QString const& airc_qsLabel);
            void m_vSetPosition(db::beams::beams_Beam::Position::type const& airc_tuplePosition);
            void m_vSetDimensions(db::beams::beams_Beam::Dimensions::type const& airc_tupleDimensions);
            void m_vSetBeam(db::beams::beams_Beam::Beam::type const&);

        // operations
        public:
            QJsonObject mc_qjoToJson()   const;
            void m_vFromJson(QJsonObject const&);

            db::beams::beams_Dokos mc_ToDokos()   const;
            void m_vFromDokos(db::beams::beams_Dokos const&);

        // friend streamers
        public:
            friend
            QDataStream& operator << (
            	QDataStream& ao_rDataStream,
            	dbproxy_Beam const& airc_DbProxyBeam
            );
            
            friend
            QDataStream& operator >> (
            	QDataStream& ao_rDataStream,
            	dbproxy_Beam& ao_rDbProxyBeam
            );
            
            friend
            QDataStream& operator << (
            	QDataStream& ao_rDataStream,
            	std::shared_ptr<dbproxy_Beam> const& airc_spDbProxyBeam
            );
            
            friend
            QDataStream& operator >> (
            	QDataStream& ao_rDataStream,
            	std::shared_ptr<dbproxy_Beam>& aor_spDbProxyBeam
            );
	};

	/*********************************************
	*
	* implementation dbproxy_BeamImpl01	
	*
	**********************************************/
	// constructors
	dbproxy_Beam::dbproxy_BeamImpl01::dbproxy_BeamImpl01(
        dbproxy_Beam* ai_pBeam,
        QString const& airc_qsStathmi,
        QSharedPointer<db::vars::vars_TInstance> ai_qspInstance
    )
		:	m_pBeam(ai_pBeam)
        ,   m_qspInstance(ai_qspInstance)
        ,   m_qspBeam(QSharedPointer<db::beams::beams_Beam>::create(airc_qsStathmi))
    {
		Q_ASSERT(m_pBeam);
        Q_ASSERT(m_qspInstance);
        Q_ASSERT(m_qspBeam);
    }

    // operations
    QJsonObject dbproxy_Beam::dbproxy_BeamImpl01::mc_qjoToJson()   const   {
        Q_ASSERT(m_qspBeam);

        return m_qspBeam->mc_qjoToJson();
    }

    void dbproxy_Beam::dbproxy_BeamImpl01::m_vFromJson(QJsonObject const& airc_qjoBeam)    {
        Q_ASSERT(m_qspBeam);

        m_qspBeam->m_vFromJson(airc_qjoBeam);
    }

    db::beams::beams_Dokos dbproxy_Beam::dbproxy_BeamImpl01::mc_ToDokos()   const {
        Q_ASSERT(m_qspBeam);

        return m_qspBeam->mc_ToDokos();
    }

    void dbproxy_Beam::dbproxy_BeamImpl01::m_vFromDokos(
        db::beams::beams_Dokos const& airc_Dokos
    )    {
        Q_ASSERT(m_qspBeam);

        m_qspBeam->m_vFromDokos(airc_Dokos);
    }

    // modifiers
    void dbproxy_Beam::dbproxy_BeamImpl01::m_vSetId(QUuid const& airc_quuidId)    {
        Q_ASSERT(m_qspBeam);
        Q_ASSERT(!airc_quuidId.isNull());

        m_qspBeam->m_vSetId(airc_quuidId);
    }

    void dbproxy_Beam::dbproxy_BeamImpl01::m_vSetLabel(QString const& airc_qsLabel)   {
        Q_ASSERT(m_qspBeam);
        Q_ASSERT(!airc_qsLabel.isEmpty());

        m_qspBeam->m_vSetLabel(airc_qsLabel);
    }

    void dbproxy_Beam::dbproxy_BeamImpl01::m_vSetPosition(db::beams::beams_Beam::Position::type const& airc_tuplePosition)    {
        Q_ASSERT(m_qspBeam);

        m_qspBeam->m_vSetPosition(airc_tuplePosition);
    }

    void dbproxy_Beam::dbproxy_BeamImpl01::m_vSetDimensions(db::beams::beams_Beam::Dimensions::type const& airc_tupleDimensions)   {
        Q_ASSERT(m_qspBeam);

        m_qspBeam->m_vSetDimensions(airc_tupleDimensions);
    }

    void dbproxy_Beam::dbproxy_BeamImpl01::m_vSetBeam(db::beams::beams_Beam::Beam::type const& airc_tupleBeam)   {
		Q_ASSERT(m_qspBeam);

        m_qspBeam->m_vSetBeam(airc_tupleBeam);
    }

    // accessors
    QUuid dbproxy_Beam::dbproxy_BeamImpl01::mc_quuidGetId()   const   {
        Q_ASSERT(m_qspBeam);

        return m_qspBeam->mc_quuidGetId();
    }

    QString dbproxy_Beam::dbproxy_BeamImpl01::mc_qsGetLabel() const   {
        Q_ASSERT(m_qspBeam);

        return m_qspBeam->mc_qsGetLabel();
    }

    db::beams::beams_Beam::Position::type dbproxy_Beam::dbproxy_BeamImpl01::mc_tupleGetPosition()    const    {
        Q_ASSERT(m_qspBeam);

        return m_qspBeam->mc_tupleGetPosition();
    }

    db::beams::beams_Beam::Dimensions::type dbproxy_Beam::dbproxy_BeamImpl01::mc_tupleGetDimensions()    const    {
        Q_ASSERT(m_qspBeam);

        return m_qspBeam->mc_tupleGetDimensions();
    }

    db::beams::beams_Beam::Beam::type dbproxy_Beam::dbproxy_BeamImpl01::mc_tupleGetBeam()   const   {
		Q_ASSERT(m_qspBeam);

        return m_qspBeam->mc_tupleGetBeam();
    }


/////////////dbproxy_Beam/////////////

// constructors
dbproxy_Beam::dbproxy_Beam(
    QString const& airc_qsStathmi,
    QSharedPointer<db::vars::vars_TInstance> ai_qspInstance
)   {
	Q_ASSERT(!m_spBeamImpl);

	m_spBeamImpl = spimpl::make_impl<dbproxy_BeamImpl01>(this, airc_qsStathmi, ai_qspInstance);
}

dbproxy_Beam::dbproxy_Beam(
    QString const& airc_qsStathmi,
    QSharedPointer<db::vars::vars_TInstance> ai_qspInstance,
    db::beams::beams_Beam::Beam::type const& airc_tupleBeam
)
    :   dbproxy_Beam{airc_qsStathmi, ai_qspInstance}
{
	Q_ASSERT(m_spBeamImpl);

	m_spBeamImpl->m_vSetBeam(airc_tupleBeam);
}

dbproxy_Beam::dbproxy_Beam(
    QString const& airc_qsStathmi,
    QSharedPointer<db::vars::vars_TInstance> ai_qspInstance,
    QJsonObject const& airc_qjoBeam
)
    :   dbproxy_Beam{airc_qsStathmi, ai_qspInstance}
{
	Q_ASSERT(m_spBeamImpl);

	m_spBeamImpl->m_vFromJson(airc_qjoBeam);
}

// operations
QJsonObject dbproxy_Beam::mc_qjoToJson()   const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_qjoToJson();
}

void dbproxy_Beam::m_vFromJson(QJsonObject const& airc_qjoBeam)    {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vFromJson(airc_qjoBeam);
}

db::beams::beams_Dokos dbproxy_Beam::mc_ToDokos()   const {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_ToDokos();
}

void dbproxy_Beam::m_vFromDokos(db::beams::beams_Dokos const& airc_Dokos)  {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vFromDokos(airc_Dokos);
}

// modifiers
void dbproxy_Beam::m_vSetId(QUuid const& airc_quuidId)    {
	Q_ASSERT(m_spBeamImpl);
    Q_ASSERT(!airc_quuidId.isNull());

    m_spBeamImpl->m_vSetId(airc_quuidId);
}

void dbproxy_Beam::m_vSetLabel(QString const& airc_qsLabel)   {
	Q_ASSERT(m_spBeamImpl);
    Q_ASSERT(!airc_qsLabel.isEmpty());

    m_spBeamImpl->m_vSetLabel(airc_qsLabel);
}

void dbproxy_Beam::m_vSetPosition(db::beams::beams_Beam::Position::type const& airc_tuplePosition)    {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vSetPosition(airc_tuplePosition);
}

void dbproxy_Beam::m_vSetDimensions(db::beams::beams_Beam::Dimensions::type const& airc_tupleDimensions)   {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vSetDimensions(airc_tupleDimensions);
}

void dbproxy_Beam::m_vSetBeam(db::beams::beams_Beam::Beam::type const& airc_tupleBeam)   {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vSetBeam(airc_tupleBeam);
}


// accessors
QUuid dbproxy_Beam::mc_quuidGetId()   const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_quuidGetId();
}

QString dbproxy_Beam::mc_qsGetLabel() const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_qsGetLabel();
}

db::beams::beams_Beam::Position::type dbproxy_Beam::mc_tupleGetPosition()    const    {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_tupleGetPosition();
}

db::beams::beams_Beam::Dimensions::type dbproxy_Beam::mc_tupleGetDimensions()    const    {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_tupleGetDimensions();
}

db::beams::beams_Beam::Beam::type dbproxy_Beam::mc_tupleGetBeam() const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_tupleGetBeam();
}

// streamers
QDataStream& operator << (
	QDataStream& ao_rDataStream,
	dbproxy_Beam const& airc_DbProxyBeam
)   {
    Q_ASSERT(airc_DbProxyBeam.m_spBeamImpl);
    Q_ASSERT(airc_DbProxyBeam.m_spBeamImpl->m_qspBeam);

	ao_rDataStream
        <<  *airc_DbProxyBeam.m_spBeamImpl->m_qspBeam
	;

    return ao_rDataStream;
}

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	dbproxy_Beam& ao_rDbProxyBeam
)   {
    Q_ASSERT(ao_rDbProxyBeam.m_spBeamImpl);
    Q_ASSERT(ao_rDbProxyBeam.m_spBeamImpl->m_qspBeam);

	ao_rDataStream
        >>  *ao_rDbProxyBeam.m_spBeamImpl->m_qspBeam
	;

    return ao_rDataStream;
}

QDataStream& operator << (
	QDataStream& ao_rDataStream,
	std::shared_ptr<dbproxy_Beam> const& airc_spDbProxyBeam
)   {
    return ao_rDataStream  <<  *airc_spDbProxyBeam;
}

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	std::shared_ptr<dbproxy_Beam>& aor_spDbProxyBeam
)   {
    return ao_rDataStream    >>  *aor_spDbProxyBeam;
}

	}	// namespace dbproxy
}	// namespace models

