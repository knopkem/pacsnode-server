#ifndef patientData_h__
#define patientData_h__

#include <iostream>

#include <QString>
#include <QStringList>
#include <QJsonObject>

#pragma warning (disable : 4251)

namespace pacsnode {

/**
 * Container element to store and transfer series related DICOM data
 */
class PatientData 
{
public:
    PatientData();

    QJsonObject toJSON() const;

    QString DD_DCM_StudyInstanceUID;
    QString DD_DCM_SeriesInstanceUID;
    QString DD_DCM_SOPInstanceUID;

    QString DD_DCM_PatientName;
    QString DD_DCM_PatientID;
    QString DD_DCM_PatientsBirthDate;
    QString DD_DCM_PatientSex;

    QString DD_DCM_ModalitiesInStudy;
    QString DD_DCM_AccessionNumber;
    QString DD_DCM_StudyDescription;
    QString DD_DCM_StudyDate;
    QString DD_DCM_StudyTime;
    QString DD_DCM_InstitutionName;
    QString DD_DCM_ReferringPhysicianName;
    QString DD_DCM_StudyRelatedInstances;
    QString DD_DCM_StudyRelatedSeries;

    QString DD_DCM_SeriesNumber;
    QString DD_DCM_BodyPartExamined;
    QString DD_DCM_PatientPosition;
    QString DD_DCM_Modality;
    QString DD_DCM_SeriesDescription;
    QString DD_DCM_SeriesDate;
    QString DD_DCM_SeriesTime;
    QString DD_DCM_ProtocolName;
    QString DD_DCM_SeriesRelatedInstances;

    QString DD_DCM_Rows;
    QString DD_DCM_Cols;

    QString DD_LocalInstances;
    QString DD_Thumbnail;
    QString DD_Provider;

    QString DD_KeyImageSOPInstanceUID;

    QStringList toList() const;

    /**
     * creates an object from an ordered list of strings
     */
    static PatientData fromList(const QStringList& input);

protected:
    static QString convertBackslashes(const QString& input);
};

}

#endif // patientData_h__

