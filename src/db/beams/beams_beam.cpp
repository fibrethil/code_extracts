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

#include "beams_beam.h"

#include "beams_dokos.h"
#include "beams_nodes.h"
#include "beams_coords.h"
#include "beams_crosssection.h"
#include "beams_supportrole.h"

#include <specs/include/domstx_beamwidth.h>
#include <specs/include/domstx_beamheight.h>

#include <QString>
#include <QtGlobal>
#include <QtDebug>
#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QPointF>
#include <QDataStream>
#include <QRegExp>

#include <cmath>

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
    char const* uc_pcId = "id";
    char const* uc_pcUuid = "uuid";
    char const* uc_pcLabel = "label";
    char const* uc_pcX1 = "x1"; 
    char const* uc_pcY1 = "y1"; 
    char const* uc_pcX2 = "x2"; 
    char const* uc_pcY2 = "y2"; 
    char const* uc_pcB0 = "b0"; 
    char const* uc_pcD0 = "d0"; 
    char const* uc_pcB = "b"; 
    char const* uc_pcStart = "start"; 
    char const* uc_pcEnd = "end"; 
    char const* uc_pcKind = "kind"; 
    char const* uc_pcGrade = "grade"; 
    char const* uc_pcCoords = "coords"; 
    char const* uc_pcDimensions = "dimensions"; 
    char const* uc_pcBoundaries = "boundaries"; 
    char const* uc_pcMaterial = "material"; 
    char const* uc_pcSupportRole = "supportRole"; 
    char const* uc_pcLoads = "loads"; 


	/*********************************************
	*
	* beams
	*
	**********************************************/

	}	// namespace unit

	namespace db		{
		namespace beams		{

	/////////////helper classes//////////////////

	/*********************************************
	*
	* interface beams_BeamImpl01
	*
	**********************************************/

	class beams_Beam::beams_BeamImpl01		{
		// data
		private:
			beams_Beam* m_pBeam;

            QUuid m_qUuid;
            QString m_qsLabel;
            QString m_qsStathmi;

            double m_dX1 {0.0};
            double m_dY1 {0.0};
            double m_dX2 {0.0};
            double m_dY2 {0.0};

            double m_dB0    {0.0};
            double m_dD0    {0.0};
            double m_dB {0.0};

            // boundaries
            int m_iStart    {0};
            int m_iEnd    {0};

            // materials
            int m_iKind    {0};
            int m_iGrade    {0};

            double m_dGCoeff    {0.0};
            double m_dG    {0.0};

            double m_dQCoeff    {0.0};
            double m_dQ    {0.0};

            double m_dGepCoeff    {0.0};
            double m_dGep    {0.0};

            beams_SupportRole m_SupportRole;
            beams_Loads m_Loads;

        // helper functions
        private:
            int mc_iGetNumber() const;
            double mc_dGetLength()  const;
	
		// constructors
		public:
			beams_BeamImpl01() = default;
			beams_BeamImpl01(beams_Beam*);
			beams_BeamImpl01(
                beams_Beam*,
                QString const& airc_qsStathmi
            );

			beams_BeamImpl01(beams_BeamImpl01 const&) = default;
			beams_BeamImpl01(beams_BeamImpl01&&) = default;
			~beams_BeamImpl01() = default;

		// operators
		public:
			beams_BeamImpl01& operator = (beams_BeamImpl01 const&) = default;
			beams_BeamImpl01& operator = (beams_BeamImpl01&&) = default;

        // accessors
        public:
            QUuid mc_quuidGetId()   const;
            QString mc_qsGetLabel() const;
            Position::type mc_tupleGetPosition()    const;
            Dimensions::type mc_tupleGetDimensions()    const;
            Beam::type mc_tupleGetBeam()   const;

        // modifiers
        public:
            void m_vSetId(QUuid const& airc_quuidId);
            void m_vSetLabel(QString const& airc_qsLabel);
            void m_vSetPosition(Position::type const& airc_tuplePosition);
            void m_vSetDimensions(Dimensions::type const& airc_tupleDimensions);
            void m_vSetBeam(Beam::type const&);

        // operations
        public:
            QJsonObject mc_qjoToJson()   const;
            void m_vFromJson(QJsonObject const&);

            beams_Dokos mc_ToDokos()  const;
            void m_vFromDokos(beams_Dokos const& airc_Dokos);
	};

	/*********************************************
	*
	* implementation beams_BeamImpl01	
	*
	**********************************************/
	// constructors
	beams_Beam::beams_BeamImpl01::beams_BeamImpl01(beams_Beam* ai_pBeam)
		:	m_pBeam(ai_pBeam)
    {
		Q_ASSERT(m_pBeam);

        qRegisterMetaType<beams_Beam>("beams_Beam");
        qRegisterMetaType<beams_Beam::LoadCombinationComponent::type>("beams_Beam_Load");
    }

	beams_Beam::beams_BeamImpl01::beams_BeamImpl01(
        beams_Beam* ai_pBeam,
        QString const& airc_qsStathmi
    )
		:	m_pBeam(ai_pBeam)
        ,   m_qsStathmi(airc_qsStathmi)
    {
		Q_ASSERT(m_pBeam);

        qRegisterMetaType<beams_Beam>("beams_Beam");
        qRegisterMetaType<beams_Beam::LoadCombinationComponent::type>("beams_Beam_Load");
    }

    // operations
    QJsonObject beams_Beam::beams_BeamImpl01::mc_qjoToJson()   const   {
        QJsonObject l_qjoBeam;

        // Id
        QJsonObject l_qjoId;
        l_qjoId.insert(uc_pcUuid, m_qUuid.toString());
        l_qjoId.insert(uc_pcLabel, m_qsLabel);
        l_qjoBeam.insert(uc_pcId, l_qjoId);

        // coords
        QJsonObject l_qjoCoords;
        l_qjoCoords.insert(uc_pcX1, m_dX1);
        l_qjoCoords.insert(uc_pcY1, m_dY1);
        l_qjoCoords.insert(uc_pcX2, m_dX2);
        l_qjoCoords.insert(uc_pcY2, m_dY2);
        l_qjoBeam.insert(uc_pcCoords, l_qjoCoords);

        // dimensions
        QJsonObject l_qjoDimensions;
        l_qjoDimensions.insert(uc_pcB0, (-1)*m_dB0);
        l_qjoDimensions.insert(uc_pcD0, m_dD0);
        l_qjoDimensions.insert(uc_pcB, m_dB);
        l_qjoBeam.insert(uc_pcDimensions, l_qjoDimensions);

        // boundaries
        QJsonObject l_qjoBoundaries;
        l_qjoBoundaries.insert(uc_pcStart, 1);
        l_qjoBoundaries.insert(uc_pcEnd, 1);
        l_qjoBeam.insert(uc_pcBoundaries, l_qjoBoundaries);

        // material
        QJsonObject l_qjoMaterial;
        l_qjoMaterial.insert(uc_pcKind, 1);
        l_qjoMaterial.insert(uc_pcGrade, 1);
        l_qjoBeam.insert(uc_pcMaterial, l_qjoMaterial);

        // support role
        l_qjoBeam.insert(uc_pcSupportRole, m_SupportRole.mc_qjoToJson());

        // loads
        l_qjoBeam.insert(uc_pcLoads, m_Loads.mc_qjoToJson());

        return l_qjoBeam;
    }

    void beams_Beam::beams_BeamImpl01::m_vFromJson(QJsonObject const& airc_qjoBeam)    {
        // top level
        auto l_qitrId = airc_qjoBeam.find(uc_pcId);
        Q_ASSERT(l_qitrId != airc_qjoBeam.end());

        auto l_qitrCoords = airc_qjoBeam.find(uc_pcCoords);
        Q_ASSERT(l_qitrCoords != airc_qjoBeam.end());

        auto l_qitrDimensions = airc_qjoBeam.find(uc_pcDimensions);
        Q_ASSERT(l_qitrDimensions != airc_qjoBeam.end());

        auto l_qitrBoundaries = airc_qjoBeam.find(uc_pcBoundaries);
        Q_ASSERT(l_qitrBoundaries != airc_qjoBeam.end());

        auto l_qitrMaterial = airc_qjoBeam.find(uc_pcMaterial);
        Q_ASSERT(l_qitrBoundaries != airc_qjoBeam.end());

        auto l_qitrSupportRole = airc_qjoBeam.find(uc_pcSupportRole);
        Q_ASSERT(l_qitrSupportRole != airc_qjoBeam.end());

        auto l_qitrLoads = airc_qjoBeam.find(uc_pcLoads);
        Q_ASSERT(l_qitrLoads != airc_qjoBeam.end());

        // id
        auto l_qjvId = l_qitrId.value();
        Q_ASSERT(l_qjvId.isObject());

        auto l_qjoId = l_qjvId.toObject();

        auto l_qjitrUuid = l_qjoId.find(uc_pcUuid);
        Q_ASSERT(l_qjoId.end() != l_qjitrUuid);

        auto l_qjvUuid = l_qjitrUuid.value();
        Q_ASSERT(l_qjvUuid.isString());

        m_qUuid = QUuid(l_qjvUuid.toString());

        auto l_qjitrLabel = l_qjoId.find(uc_pcLabel);
        Q_ASSERT(l_qjoId.end() != l_qjitrLabel);

        auto l_qjvLabel = l_qjitrLabel.value();
        Q_ASSERT(l_qjvLabel.isString());

        m_qsLabel = l_qjvLabel.toString();

        // coords
        auto l_qjvCoords = l_qitrCoords.value();
        Q_ASSERT(l_qjvCoords.isObject());

        auto l_qjoCoords = l_qjvCoords.toObject();

        auto l_itrX1 = l_qjoCoords.find(uc_pcX1);
        auto l_qjvX1 = l_itrX1.value();
        Q_ASSERT(l_qjvX1.isDouble());

        m_dX1 = l_qjvX1.toDouble();

        auto l_itrY1 = l_qjoCoords.find(uc_pcY1);
        auto l_qjvY1 = l_itrY1.value();
        Q_ASSERT(l_qjvY1.isDouble());

        m_dY1 = l_qjvY1.toDouble();

        auto l_itrX2 = l_qjoCoords.find(uc_pcX2);
        auto l_qjvX2 = l_itrX2.value();
        Q_ASSERT(l_qjvX2.isDouble());

        m_dX2 = l_qjvX2.toDouble();

        auto l_itrY2 = l_qjoCoords.find(uc_pcY2);
        auto l_qjvY2 = l_itrY2.value();
        Q_ASSERT(l_qjvY2.isDouble());

        m_dY2 = l_qjvY2.toDouble();

        // dimensions
        auto l_qjvDimensions = l_qitrDimensions.value();
        Q_ASSERT(l_qjvDimensions.isObject());

        auto l_qjoDimensions = l_qjvDimensions.toObject();

        auto l_itrB0 = l_qjoDimensions.find(uc_pcB0);
        auto l_qjvB0 = l_itrB0.value();
        Q_ASSERT(l_qjvB0.isDouble());

        m_dB0 = (-1)*l_qjvB0.toDouble();

        auto l_itrD0 = l_qjoDimensions.find(uc_pcD0);
        auto l_qjvD0 = l_itrD0.value();
        Q_ASSERT(l_qjvD0.isDouble());

        m_dD0 = l_qjvD0.toDouble();

        auto l_itrB = l_qjoDimensions.find(uc_pcB);
        auto l_qjvB = l_itrB.value();
        Q_ASSERT(l_qjvB.isDouble());

        m_dB = l_qjvB.toDouble();

        // boundaries
        auto l_qjvBoundaries = l_qitrBoundaries.value();
        Q_ASSERT(l_qjvBoundaries.isObject());

        auto l_qjoBoundaries = l_qjvBoundaries.toObject();

        auto l_itrStart = l_qjoBoundaries.find(uc_pcStart);
        auto l_qjvStart = l_itrStart.value();
        Q_ASSERT(l_qjvStart.isDouble());

        m_iStart = l_qjvStart.toInt();

        auto l_itrEnd = l_qjoBoundaries.find(uc_pcEnd);
        auto l_qjvEnd = l_itrEnd.value();
        Q_ASSERT(l_qjvEnd.isDouble());

        m_iEnd = l_qjvEnd.toInt();

        // materials
        auto l_qjvMaterial = l_qitrMaterial.value();
        Q_ASSERT(l_qjvMaterial.isObject());

        auto l_qjoMaterial = l_qjvMaterial.toObject();

        auto l_itrKind = l_qjoMaterial.find(uc_pcKind);
        auto l_qjvKind = l_itrKind.value();
        Q_ASSERT(l_qjvKind.isDouble());

        m_iKind = l_qjvKind.toInt();

        auto l_itrGrade = l_qjoMaterial.find(uc_pcGrade);
        auto l_qjvGrade = l_itrGrade.value();
        Q_ASSERT(l_qjvGrade.isDouble());

        m_iGrade = l_qjvGrade.toInt();

        // support role
        auto l_qjvSupportRole = l_qitrSupportRole.value();
        Q_ASSERT(l_qjvSupportRole.isObject());

        m_SupportRole = beams_SupportRole(l_qjvSupportRole.toObject());

        // loads
        auto l_qjvLoads = l_qitrLoads.value();
        Q_ASSERT(l_qjvLoads.isObject());

        m_Loads = beams_Loads(l_qjvLoads.toObject());

    }

    beams_Dokos beams_Beam::beams_BeamImpl01::mc_ToDokos()  const {
        beams_Dokos l_Dokos;

        l_Dokos.m_vSetDokos(
            std::make_tuple(
                mc_iGetNumber(),
                m_iKind,
                mc_dGetLength(),
                beams_Nodes(m_iStart, m_iEnd),
                beams_Coords(m_dX1, m_dY1, m_dX2, m_dY2),
                beams_CrossSection(m_dB0, m_dD0),
                m_SupportRole,
                m_Loads
            )
        );

        return l_Dokos;
    }

    void beams_Beam::beams_BeamImpl01::m_vFromDokos(beams_Dokos const& airc_Dokos)    {
        m_qsLabel = QObject::tr("B ") + QString::number(airc_Dokos.mc_iGetNumber());
        m_iKind = airc_Dokos.mc_iGetType();

        auto l_Nodes = airc_Dokos.mc_GetNodes();
        m_iStart = l_Nodes.mc_iGetStartNode();
        m_iEnd = l_Nodes.mc_iGetEndNode();

        auto l_Coords = airc_Dokos.mc_GetCoords();
        m_dX1 = l_Coords.mc_dGetX1();
        m_dY1 = l_Coords.mc_dGetY1();
        m_dX2 = l_Coords.mc_dGetX2();
        m_dY2 = l_Coords.mc_dGetY2();

        auto l_CrossSection = airc_Dokos.mc_GetCrossSection();
        m_dB0 = l_CrossSection.mc_dGetB0();
        m_dD0 = l_CrossSection.mc_dGetD0();

        m_SupportRole = airc_Dokos.mc_GetSupportRole();
        m_Loads = airc_Dokos.mc_GetLoads();
    }

    // modifiers
    void beams_Beam::beams_BeamImpl01::m_vSetId(QUuid const& airc_quuidId)    {
        Q_ASSERT(!airc_quuidId.isNull());

        m_qUuid = airc_quuidId;
    }

    void beams_Beam::beams_BeamImpl01::m_vSetLabel(QString const& airc_qsLabel)   {
        Q_ASSERT(!airc_qsLabel.isEmpty());

        m_qsLabel = airc_qsLabel;
    }

    void beams_Beam::beams_BeamImpl01::m_vSetPosition(Position::type const& airc_tuplePosition)   {
        auto [l_qStartPoint, l_qEndPoint] = airc_tuplePosition;

        m_dX1 = l_qStartPoint.x();
        m_dY1 = l_qStartPoint.y();

        m_dX2 = l_qEndPoint.x();
        m_dY2 = l_qEndPoint.y();
    }

    void beams_Beam::beams_BeamImpl01::m_vSetDimensions(Dimensions::type const& airc_tupleDimensions) {
        auto [l_tupleCrossSection, l_dBeamPlateContribution] = airc_tupleDimensions;
        m_dB = l_dBeamPlateContribution;

        auto [l_dB0, l_dD0] = l_tupleCrossSection;
        m_dB0 = l_dB0;
        m_dD0 = l_dD0;
    }

    void beams_Beam::beams_BeamImpl01::m_vSetBeam(Beam::type const& airc_tupleBeam)   {
		Q_ASSERT(m_pBeam);

        auto [
                l_qUuid
             ,  l_qsLabel
             ,  l_tuplePosition
             ,  l_tupleDimensions
             ,  l_tupleBoundaries
             ,  l_tupleMaterial
             ,  l_SupportRole
             ,  l_Loads
        ] = airc_tupleBeam;

        m_qUuid = l_qUuid;
        m_qsLabel = l_qsLabel;

        auto [l_qPointStart, l_qPointEnd] = l_tuplePosition;
        auto [l_qCrossSection, l_dBPlateContribution] = l_tupleDimensions;
        auto [l_iBoundaryStart, l_iBoundaryEnd] = l_tupleBoundaries;
        auto [l_iKind, l_iGrade] = l_tupleMaterial;

        m_dX1 = l_qPointStart.x();
        m_dY1 = l_qPointStart.y();

        m_dX2 = l_qPointEnd.x();
        m_dY2 = l_qPointEnd.y();

        auto [l_dB0, l_dD0] = l_qCrossSection;
        m_dB0 = l_dB0;
        m_dD0 = l_dD0;
        m_dB = l_dBPlateContribution;

        m_SupportRole = l_SupportRole;
        m_Loads = l_Loads;
    }

    // accessors
    QUuid beams_Beam::beams_BeamImpl01::mc_quuidGetId()   const   {
        Q_ASSERT(!m_qUuid.isNull());

        return m_qUuid;
    }

    QString beams_Beam::beams_BeamImpl01::mc_qsGetLabel() const   {
        Q_ASSERT(!m_qsLabel.isEmpty());

        return m_qsLabel;
    }

    beams_Beam::Position::type beams_Beam::beams_BeamImpl01::mc_tupleGetPosition()    const   {
        return  {
                QPointF(m_dX1, m_dY1),
                QPointF(m_dX2, m_dY2)
            }
        ;
    }

    beams_Beam::Dimensions::type beams_Beam::beams_BeamImpl01::mc_tupleGetDimensions()    const   {
        return  {
                std::make_tuple(m_dB0, m_dD0),
                m_dB
            }
        ;
    }

    beams_Beam::Beam::type
    beams_Beam::beams_BeamImpl01::mc_tupleGetBeam()   const   {
        auto l_tuplePosition = std::tuple(QPointF(m_dX1, m_dY1), QPointF(m_dX2, m_dY2));
        auto l_tupleDimensions = std::tuple(std::tuple(m_dB0, m_dD0), m_dB );

        return  {
            m_qUuid,
            m_qsLabel,
            l_tuplePosition,
            l_tupleDimensions,
            std::tuple(1, 1),
            std::tuple(1, 3),
            m_SupportRole,
            m_Loads
        };
    }

    // helper functions
    int beams_Beam::beams_BeamImpl01::mc_iGetNumber() const   {
        Q_ASSERT(!m_qsLabel.isEmpty());

        auto l_qslLabel = m_qsLabel.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        Q_ASSERT(2 == l_qslLabel.size());

        auto l_bOk  {false};
        auto l_iNumber = l_qslLabel[1].toInt(&l_bOk);
        Q_ASSERT(l_bOk); 

        return l_iNumber;
    }

    double beams_Beam::beams_BeamImpl01::mc_dGetLength()  const   {
        std::hypot(m_dX2-m_dX1, m_dY2-m_dY1);
    }


/////////////beams_Beam/////////////

// constructors
beams_Beam::beams_Beam()   {
	Q_ASSERT(!m_spBeamImpl);

	m_spBeamImpl = spimpl::make_impl<beams_BeamImpl01>(this);
}

beams_Beam::beams_Beam(
    QString const& airc_qsStathmi
)   {
	Q_ASSERT(!m_spBeamImpl);

	m_spBeamImpl = spimpl::make_impl<beams_BeamImpl01>(this, airc_qsStathmi);
}

beams_Beam::beams_Beam(
    QString const& airc_qsStathmi,
    Beam::type const& airc_tupleBeam
)
    :   beams_Beam{airc_qsStathmi}
{
	Q_ASSERT(m_spBeamImpl);

	m_spBeamImpl->m_vSetBeam(airc_tupleBeam);
}

beams_Beam::beams_Beam(
    QString const& airc_qsStathmi,
    QJsonObject const& airc_qjoBeam
)
    :   beams_Beam{airc_qsStathmi}
{
	Q_ASSERT(m_spBeamImpl);

	m_spBeamImpl->m_vFromJson(airc_qjoBeam);
}

beams_Beam::beams_Beam(
    QUuid const& airc_quuidId,
    QString const& airc_qsStathmi,
    beams_Dokos const& airc_Dokos
)
    :   beams_Beam{airc_qsStathmi}
{
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vSetId(airc_quuidId);
	m_spBeamImpl->m_vFromDokos(airc_Dokos);
}

// operations
QJsonObject beams_Beam::mc_qjoToJson()   const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_qjoToJson();
}

void beams_Beam::m_vFromJson(QJsonObject const& airc_qjoBeam)    {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vFromJson(airc_qjoBeam);
}

beams_Dokos beams_Beam::mc_ToDokos()  const {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_ToDokos();
}

void beams_Beam::m_vFromDokos(beams_Dokos const& airc_Dokos)    {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vFromDokos(airc_Dokos);
}

// modifiers
void beams_Beam::m_vSetId(QUuid const& airc_quuidId)    {
	Q_ASSERT(m_spBeamImpl);
    Q_ASSERT(!airc_quuidId.isNull());

    m_spBeamImpl->m_vSetId(airc_quuidId);
}

void beams_Beam::m_vSetLabel(QString const& airc_qsLabel)   {
	Q_ASSERT(m_spBeamImpl);
    Q_ASSERT(!airc_qsLabel.isEmpty());

    m_spBeamImpl->m_vSetLabel(airc_qsLabel);
}

void beams_Beam::m_vSetPosition(Position::type const& airc_tuplePosition)   {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vSetPosition(airc_tuplePosition);
}

void beams_Beam::m_vSetDimensions(Dimensions::type const& airc_tupleDimensions) {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vSetDimensions(airc_tupleDimensions);
}

void beams_Beam::m_vSetBeam(Beam::type const& airc_tupleBeam)   {
	Q_ASSERT(m_spBeamImpl);

    m_spBeamImpl->m_vSetBeam(airc_tupleBeam);
}

// accessors
QUuid beams_Beam::mc_quuidGetId()   const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_quuidGetId();
}

QString beams_Beam::mc_qsGetLabel() const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_qsGetLabel();
}

beams_Beam::Position::type beams_Beam::mc_tupleGetPosition()    const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_tupleGetPosition();
}

beams_Beam::Dimensions::type beams_Beam::mc_tupleGetDimensions()    const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_tupleGetDimensions();
}

beams_Beam::Beam::type
beams_Beam::mc_tupleGetBeam() const   {
	Q_ASSERT(m_spBeamImpl);

    return m_spBeamImpl->mc_tupleGetBeam();
}

// streamers
QDataStream& operator << (
	QDataStream& ao_rDataStream,
	beams_Beam const& airc_Beam
)   {
    auto [
            l_quuidId,
            l_qsLabel,
            l_tuplePosition,
            l_tupleDimensions,
            l_tupleBoundaries,
            l_tupleMaterial,
            l_SupportRole,
            l_Loads
        ] = airc_Beam.mc_tupleGetBeam()
    ;

    auto [l_tupleStartPointF, l_tupleEndPointF] = l_tuplePosition;

    auto l_dX1 = l_tupleStartPointF.x();
    auto l_dY1 = l_tupleStartPointF.y();

    auto l_dX2 = l_tupleEndPointF.x();
    auto l_dY2 = l_tupleEndPointF.y();

    auto [l_tupleCrossSection, l_dB] = l_tupleDimensions;
    auto [l_dB0, l_dD0] = l_tupleCrossSection;

    auto l_dGCoeff = 1.35;
    auto l_dG = l_Loads.mc_dGetG() + l_Loads.mc_dGetGWall();;

    auto l_dQCoeff = 1.5;
    auto l_dQ = l_Loads.mc_dGetQ();

    auto l_dGepCoeff = 1.35;
    auto l_dGep = 0.25*0.5*2.4;


	ao_rDataStream
        <<  l_quuidId
        <<  l_qsLabel
		<<	l_dX1   << l_dY1
		<<	l_dX2   << l_dY2
		<<	l_dB0   << l_dD0   <<	l_dB
        <<  l_dGCoeff   <<  l_dG
        <<  l_dQCoeff   <<  l_dQ
        <<  l_dGepCoeff   <<  l_dGep
        <<  l_SupportRole
        <<  l_Loads
	;

    return ao_rDataStream;
}

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	beams_Beam& ao_rBeam
)   {
    QUuid l_quuidId;
    QString l_qsLabel;
    double l_dX1, l_dY1;
    double l_dX2, l_dY2;
    double l_dB0, l_dD0, l_dB;
    double l_dGCoeff, l_dG;
    double l_dQCoeff, l_dQ;
    double l_dGepCoeff, l_dGep;
    beams_SupportRole l_SupportRole;
    beams_Loads l_Loads;
	ao_rDataStream
        >>  l_quuidId
        >>  l_qsLabel
	    >>	l_dX1   >>  l_dY1
	    >>	l_dX2   >>  l_dY2
	    >>	l_dB0   >>  l_dD0   >>	l_dB
        >>  l_dGCoeff   >>  l_dG
        >>  l_dQCoeff   >>  l_dQ
        >>  l_dGepCoeff   >>  l_dGep
        >>  l_SupportRole
        >>  l_Loads
	;

    ao_rBeam.m_vSetBeam(
        std::tuple(
            l_quuidId,
            l_qsLabel,
            std::tuple(QPointF(l_dX1, l_dY1), QPointF(l_dX2, l_dY2)),
            std::tuple(std::tuple(l_dB0, l_dD0), l_dB),
            std::tuple(1, 1),
            std::tuple(1, 1),
            l_SupportRole,
            l_Loads
        )
    );

    return ao_rDataStream;
}

QDataStream& operator << (
	QDataStream& ao_rDataStream,
	std::shared_ptr<beams_Beam> const& airc_spBeam
)   {
    return ao_rDataStream  <<  *airc_spBeam;
}

QDataStream& operator >> (
	QDataStream& ao_rDataStream,
	std::shared_ptr<beams_Beam>& aor_spBeam
)   {
    return ao_rDataStream    >>  *aor_spBeam;
}

	}	// namespace beams
}	// namespace db

