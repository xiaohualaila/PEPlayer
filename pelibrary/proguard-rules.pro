# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in D:\Android\sdk/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}
-libraryjars 'C:\Program Files\Java\jdk1.8.0_152\jre\lib\rt.jar'
-libraryjars 'C:\Users\Administrator\AppData\Local\Android\Sdk\platforms\android-26\android.jar'

-optimizationpasses 5
-dontusemixedcaseclassnames

# -keep public class * extends android.app.Activity

-keep class panoeye.pelibrary.PEManager {
    public <fields>;
    public <methods>;
}

-keep class panoeye.pelibrary.PEError {
    *;
}
-keep enum panoeye.pelibrary.PEDecodeManagerStatus {
    public <fields>;
    public <methods>;
}


