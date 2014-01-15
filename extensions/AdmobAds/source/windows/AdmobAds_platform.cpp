/*
 * windows-specific implementation of the AdmobAds extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "AdmobAds_internal.h"

s3eResult AdmobAdsInit_platform()
{
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void AdmobAdsTerminate_platform()
{
    // Add any platform-specific termination code here
}

s3eResult InitAds_platform(const char* pub_id)
{
    return S3E_RESULT_SUCCESS; // S3E_RESULT_ERROR;
}

s3eResult ShowAds_platform()
{
    return S3E_RESULT_SUCCESS; // S3E_RESULT_ERROR;
}

s3eResult HideAds_platform()
{
    return S3E_RESULT_SUCCESS; // S3E_RESULT_ERROR;
}

bool hasAdLoaded_platform()
{
    return false;
}
