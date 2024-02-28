# Verificar si la carpeta pruebas existe
if [ ! -d "pruebas" ]; then
    # Si no existe, crearla
    mkdir pruebas
    echo "Carpeta 'pruebas' creada."
else
    echo "La carpeta 'pruebas' ya existe."
fi
echo "Hola  que tal..." > pruebas/f1.dat ; seq 1    1000 >> pruebas/f1.dat

./mysha256-repo  pruebas/f1.dat  repositorio1.rep

sha256sum  pruebas/f1.dat

hexdump  -C repositorio1.rep |  head  -20
