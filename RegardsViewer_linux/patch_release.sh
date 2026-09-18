patchelf --force-rpath --set-rpath '$ORIGIN' ./Release/RegardsLite
patchelf --force-rpath --set-rpath '$ORIGIN' ./Release/*.so.* 
#patchelf --force-rpath --set-rpath '.' ./Release/RegardsViewer 
#patchelf --force-rpath --set-rpath '.' ./Release/*.so.* 
