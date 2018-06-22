// Copyright eeGeo Ltd (2012-2017), All Rights Reserved

package com.eegeo.interiorsposition.senionlab;

public class SenionLabBroadcastReceiverJniMethods
{
    public static native void DidUpdateLocation(long nativeCallerPointer,
                                                double latitudeDegrees,
                                                double longitudeDegrees,
                                                double horizontalAccuracyInMeters,
                                                int senionFloorNumber);

    public static native void SetIsAuthorized(long nativeCallerPointer, boolean isAuthorized);
}
