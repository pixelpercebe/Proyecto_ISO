# Verificar si la carpeta pruebas existe
if [ ! -d "pruebas" ]; then
    # Si no existe, crearla
    mkdir pruebas
    echo "Carpeta 'pruebas' creada."
else
    echo "La carpeta 'pruebas' ya existe."
fi

./mysha256-repo E pruebas/f1.dat  repositorio1.rep

sha256sum  pruebas/f1.dat

hexdump  -C repositorio1.rep |head  -20
