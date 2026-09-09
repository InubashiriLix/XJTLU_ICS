# Simple Counter — Java Android starter

A beginner project with one Java Activity and one XML screen. Tap **Add 1**
to increase the number, or **Reset** to return to zero. The count survives
screen rotation; a fresh launch starts at zero.

## Requirements

- Android Studio with Android Gradle Plugin 9.1 support.
- JDK 17 or newer (the project was built with JDK 21).
- Android SDK Platform 36 and Build Tools 36.0.0.
- An emulator or device running **Android 7.0 (API 24) or newer**.

`minSdk 24` sets the oldest supported Android version. `compileSdk 36` and
`targetSdk 36` let the project build with a modern SDK; they do not raise that
minimum. See [Android's SDK version documentation](https://developer.android.com/guide/topics/manifest/uses-sdk-element).

## Run

1. Open this `lab1` folder in Android Studio.
2. Let Gradle sync and install the required SDK components if prompted.
3. Select an Android 7.0+ emulator or connected device.
4. Run the `app` configuration.

Or, with `ANDROID_HOME` pointing to your SDK:

```sh
./gradlew assembleDebug
adb install -r app/build/outputs/apk/debug/app-debug.apk
adb shell am start -n com.example.simplecounter/.MainActivity
```

On Windows, use `gradlew.bat assembleDebug`.

## Files to learn from

- `app/src/main/java/com/example/simplecounter/MainActivity.java`: button clicks,
  updating text, and saving state during rotation.
- `app/src/main/res/layout/activity_main.xml`: screen layout.
- `app/src/main/res/values/strings.xml`: visible text.
- `app/build.gradle`: Java and Android SDK settings.

No external app libraries, Kotlin source, or network permissions are needed.

## Check

```sh
./gradlew assembleDebug lintDebug
```

Manual check: Add 1 three times (shows 3), rotate the device (still 3),
then Reset (shows 0).
