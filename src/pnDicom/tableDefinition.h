#ifndef tableDefinition_h__
#define tableDefinition_h__

#include <QList>

#include "tableElement.h"

namespace pacsnode {

    // private field to be used for filename
    #define DCM_PrivateFileName                            DcmTagKey(0x0011, 0x0011)

    static inline QList<TableElement> supportedAttributes() {
        QList<TableElement> result;

        // PATIENT
        result.push_back( TableElement( DCM_PatientBirthDate,                      PATIENT_LEVEL,  OPTIONAL_KEY,   DATE_CLASS      ));
        result.push_back( TableElement( DCM_PatientSex,                            PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_PatientName,                           PATIENT_LEVEL,  REQUIRED_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_PatientID,                             PATIENT_LEVEL,  REQUIRED_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_PatientBirthTime,                      PATIENT_LEVEL,  OPTIONAL_KEY,   TIME_CLASS      ));
        //result.push_back( TableElement( DCM_OtherPatientIDs,                       PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_OtherPatientNames,                     PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_EthnicGroup,                           PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_PatientComments,                       PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_NumberOfPatientRelatedStudies,         PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_NumberOfPatientRelatedSeries,          PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_NumberOfPatientRelatedInstances,       PATIENT_LEVEL,  OPTIONAL_KEY,   STRING_CLASS    ));

        // STUDY
        result.push_back( TableElement( DCM_StudyDate,                             STUDY_LEVEL,    REQUIRED_KEY,   DATE_CLASS      ));
        result.push_back( TableElement( DCM_StudyTime,                             STUDY_LEVEL,    REQUIRED_KEY,   TIME_CLASS      ));
        result.push_back( TableElement( DCM_StudyID,                               STUDY_LEVEL,    REQUIRED_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_AccessionNumber,                       STUDY_LEVEL,    REQUIRED_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_ReferringPhysicianName,                STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_StudyDescription,                      STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_NameOfPhysiciansReadingStudy,          STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_StudyInstanceUID,                      STUDY_LEVEL,    UNIQUE_KEY,     UID_CLASS       ));
        result.push_back( TableElement( DCM_RETIRED_OtherStudyNumbers,             STUDY_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_AdmittingDiagnosesDescription,         STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_PatientAge,                            STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_PatientSize,                           STUDY_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_PatientWeight,                         STUDY_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_Occupation,                            STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_AdditionalPatientHistory,              STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_NumberOfStudyRelatedSeries,            STUDY_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_NumberOfStudyRelatedInstances,         STUDY_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_ModalitiesInStudy,                     STUDY_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));

        // SERIES
        result.push_back( TableElement( DCM_SeriesNumber,                          SERIES_LEVEL,    REQUIRED_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_SeriesInstanceUID,                     SERIES_LEVEL,    UNIQUE_KEY,     UID_CLASS       ));
        result.push_back( TableElement( DCM_Modality,                              SERIES_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_SeriesDescription,                     SERIES_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_SeriesDate,                            SERIES_LEVEL,    OPTIONAL_KEY,   DATE_CLASS      ));
        result.push_back( TableElement( DCM_SeriesTime,                            SERIES_LEVEL,    OPTIONAL_KEY,   TIME_CLASS      ));
        result.push_back( TableElement( DCM_BodyPartExamined,                      SERIES_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_PatientPosition,                       SERIES_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_ProtocolName,                          SERIES_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));
        result.push_back( TableElement( DCM_NumberOfSeriesRelatedInstances,        SERIES_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));


        // IMAGE
        result.push_back( TableElement( DCM_InstanceNumber,                        IMAGE_LEVEL,    REQUIRED_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_SOPInstanceUID,                        IMAGE_LEVEL,    UNIQUE_KEY,     UID_CLASS       ));
        result.push_back( TableElement( DCM_SliceLocation,                         IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_ImageType     ,                        IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_NumberOfFrames,                        IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_Rows,                                  IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_Columns,                               IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_WindowWidth,                           IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_WindowCenter,                          IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_PhotometricInterpretation,             IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_RescaleSlope,                          IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_RescaleIntercept,                      IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_PixelSpacing,                          IMAGE_LEVEL,    OPTIONAL_KEY,   OTHER_CLASS     ));

        result.push_back( TableElement( DCM_SOPClassUID,                           IMAGE_LEVEL,    REQUIRED_KEY,   OTHER_CLASS     ));
        result.push_back( TableElement( DCM_PrivateFileName,                       IMAGE_LEVEL,    OPTIONAL_KEY,   STRING_CLASS    ));


        return result;
    }

}
#endif // tableDefinition_h__

