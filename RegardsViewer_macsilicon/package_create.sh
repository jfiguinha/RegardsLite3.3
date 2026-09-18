if [ -d package ]; then echo "Exists"; else mkdir package;fi
cp -r ./Release/RegardsLite.app ./package/RegardsLite/
rm -f ./package/RegardsLite.dmg
echo "Create Package"
hdiutil create -fs HFS+ -srcfolder ./package/RegardsLite -volname RegardsLite ./package/RegardsLite.dmg
