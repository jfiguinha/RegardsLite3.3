#/bin/bash
./MacOsLibExplorer ./Release/RegardsLite.app/Contents/MacOS ./Release/RegardsLite.app/Contents/Frameworks change_dylib.sh 1
./MacOsLibExplorer ./Release/RegardsLite.app/Contents/Frameworks ./Release/RegardsLite.app/Contents/Frameworks change_dylib.sh 1
codesign -fs - ./Release/RegardsLite.app --deep
