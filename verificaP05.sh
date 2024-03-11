
# Verificar si la carpeta pruebas existe
if [ ! -d "pruebas" ]; then
    # Si no existe, crearla
    mkdir pruebas
    echo "Carpeta 'pruebas' creada."
else
    echo "La carpeta 'pruebas' ya existe."
fi
echo "Hello how...This is  a new test" > pruebas/f1-3.dat ; seq 1 124 >> pruebas/f1-3.dat

# execute mysha256-repo to Insert a file (I)  ("I" is a new argument of mysha256-repo.)
./mysha256-repo  I  pruebas/f1-3.dat repositorio1.rep

sha256sum  pruebas/f1-3.dat

hexdump -C repositorio1.rep | less
