#include "patientData.h"

#include <QDebug>
#include <QJsonObject>

namespace pacsnode {

PatientData::PatientData() : DD_DCM_StudyInstanceUID(""), DD_DCM_SeriesInstanceUID("")
{

}

//--------------------------------------------------------------------------------------

QJsonObject PatientData::toJSON() const
{
    QJsonObject json;
    json.insert("DCM_StudyInstanceUID", DD_DCM_StudyInstanceUID);
    json.insert("DCM_SeriesInstanceUID", DD_DCM_SeriesInstanceUID);
    json.insert("DCM_SOPInstanceUID", DD_DCM_SOPInstanceUID);

    json.insert("DCM_PatientName", convertBackslashes(DD_DCM_PatientName));
    json.insert("DCM_PatientID", convertBackslashes(DD_DCM_PatientID));
    json.insert("DCM_PatientsBirthDate", DD_DCM_PatientsBirthDate);
    json.insert("DCM_PatientSex", DD_DCM_PatientSex);

    json.insert("DCM_ModalitiesInStudy", convertBackslashes(DD_DCM_ModalitiesInStudy));
    json.insert("DCM_AccessionNumber", convertBackslashes(DD_DCM_AccessionNumber));
    json.insert("DCM_StudyDescription", convertBackslashes(DD_DCM_StudyDescription));
    json.insert("DCM_StudyDate", convertBackslashes(DD_DCM_StudyDate));
    json.insert("DCM_StudyTime", convertBackslashes(DD_DCM_StudyTime));
    json.insert("DCM_InstitutionName", convertBackslashes(DD_DCM_InstitutionName));
    json.insert("DCM_ReferringPhysicianName", convertBackslashes(DD_DCM_ReferringPhysicianName));
    json.insert("DCM_NumberOfStudyRelatedInstances", DD_DCM_StudyRelatedInstances);
    json.insert("DCM_NumberOfStudyRelatedSeries", DD_DCM_StudyRelatedSeries);

    json.insert("DCM_SeriesNumber", DD_DCM_SeriesNumber);
    json.insert("DCM_BodyPartExamined", convertBackslashes(DD_DCM_BodyPartExamined));
    json.insert("DCM_PatientPosition", convertBackslashes(DD_DCM_PatientPosition));
    json.insert("DCM_Modality", convertBackslashes(DD_DCM_Modality));
    json.insert("DCM_SeriesDescription", convertBackslashes(DD_DCM_SeriesDescription));
    json.insert("DCM_SeriesDate", convertBackslashes(DD_DCM_SeriesDate));
    json.insert("DCM_SeriesTime", convertBackslashes(DD_DCM_SeriesTime));
    json.insert("DCM_ProtocolName", convertBackslashes(DD_DCM_ProtocolName));
    json.insert("DCM_Thumbnail", convertBackslashes(DD_Thumbnail));
    json.insert("DCM_DicomSource", convertBackslashes(DD_Provider));
    json.insert("DCM_KeyImageSOPInstanceUID", DD_KeyImageSOPInstanceUID);
    json.insert("DCM_NumberOfSeriesRelatedInstances", DD_DCM_SeriesRelatedInstances);
    json.insert("DCM_Rows", DD_DCM_Rows);
    json.insert("DCM_Cols", DD_DCM_Cols);
    json.insert("LocalInstances", DD_LocalInstances);
    return json;
}

//--------------------------------------------------------------------------------------

QString PatientData::convertBackslashes( const QString& input )
{
    QString result = input;
    std::replace( result.begin(), result.end(), QChar('\\'), QChar('/') );
    return result;
}

//--------------------------------------------------------------------------------------

QStringList PatientData::toList() const
{
    QStringList result;
    result << this->DD_DCM_StudyInstanceUID;
    result << this->DD_DCM_SeriesInstanceUID;
    result << this->DD_DCM_SOPInstanceUID;

    result << this->DD_DCM_PatientName;
    result << this->DD_DCM_PatientID;
    result << this->DD_DCM_PatientsBirthDate;
    result << this->DD_DCM_PatientSex;

    result << this->DD_DCM_ModalitiesInStudy;
    result << this->DD_DCM_AccessionNumber;
    result << this->DD_DCM_StudyDescription;
    result << this->DD_DCM_StudyDate;
    result << this->DD_DCM_StudyTime;
    result << this->DD_DCM_InstitutionName;
    result << this->DD_DCM_ReferringPhysicianName;
    result << this->DD_DCM_StudyRelatedInstances;
    result << this->DD_DCM_StudyRelatedSeries;

    result << this->DD_DCM_SeriesNumber;
    result << this->DD_DCM_BodyPartExamined;
    result << this->DD_DCM_PatientPosition;
    result << this->DD_DCM_Modality;
    result << this->DD_DCM_SeriesDescription;
    result << this->DD_DCM_SeriesDate;
    result << this->DD_DCM_SeriesTime;
    result << this->DD_DCM_ProtocolName;
    result << this->DD_DCM_SeriesRelatedInstances;

    result << this->DD_DCM_Rows;
    result << this->DD_DCM_Cols;

    result << this->DD_LocalInstances;
    result << this->DD_Thumbnail;
    result << this->DD_Provider;

    result << this->DD_KeyImageSOPInstanceUID;

    return result;
}

PatientData PatientData::fromList( const QStringList& input )
{
    PatientData data;

    QList< QString >::ConstIterator iter = input.begin();
    data.DD_DCM_StudyInstanceUID            = (*iter++);
    data.DD_DCM_SeriesInstanceUID           = (*iter++);
    data.DD_DCM_SOPInstanceUID              = (*iter++);

    data.DD_DCM_PatientName                 = (*iter++);
    data.DD_DCM_PatientID                   = (*iter++);
    data.DD_DCM_PatientsBirthDate           = (*iter++);
    data.DD_DCM_PatientSex                  = (*iter++);

    data.DD_DCM_ModalitiesInStudy           = (*iter++);
    data.DD_DCM_AccessionNumber             = (*iter++);
    data.DD_DCM_StudyDescription            = (*iter++);
    data.DD_DCM_StudyDate                   = (*iter++);
    data.DD_DCM_StudyTime                   = (*iter++);
    data.DD_DCM_InstitutionName             = (*iter++);
    data.DD_DCM_ReferringPhysicianName      = (*iter++);
    data.DD_DCM_StudyRelatedInstances       = (*iter++);
    data.DD_DCM_StudyRelatedSeries          = (*iter++);

    data.DD_DCM_SeriesNumber                = (*iter++);
    data.DD_DCM_BodyPartExamined            = (*iter++);
    data.DD_DCM_PatientPosition             = (*iter++);
    data.DD_DCM_Modality                    = (*iter++);
    data.DD_DCM_SeriesDescription           = (*iter++);
    data.DD_DCM_SeriesDate                  = (*iter++);
    data.DD_DCM_SeriesTime                  = (*iter++);
    data.DD_DCM_ProtocolName                = (*iter++);
    data.DD_DCM_SeriesRelatedInstances      = (*iter++);

    data.DD_DCM_Rows                        = (*iter++);
    data.DD_DCM_Cols                        = (*iter++);

    data.DD_LocalInstances                  = (*iter++);
    data.DD_Thumbnail                       = (*iter++);
    data.DD_Provider                        = (*iter++);
    data.DD_KeyImageSOPInstanceUID          = (*iter++);

    return data;
}

//--------------------------------------------------------------------------------------

}
