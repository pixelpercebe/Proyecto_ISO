#!/bin/bash

# Directorio de pruebas y archivo de repositorio
TEST_DIR="pruebas"
TEST_EMPTY_DIR="pruebasVacias"
REPO_FILE="repositorio1.rep"
REPO_DIR_FILE="repositorio2.rep"
TEST_FILE="f1.dat"
TEST_EMPTY_FILE="f2.dat"
TEST_FIRST_FILE="f0.dat"

# Limpiar el repositorio antes de las pruebas
rm -f "$REPO_FILE"
rm -f "$REPO_DIR_FILE"

# Verificar si la carpeta pruebas existe, Crear directorio de pruebas y archivo si no existen
if [ ! -d "pruebas" ]; then
    # Si no existe, crearla
    mkdir -p "$TEST_DIR"
    echo "Carpeta 'pruebas' creada."
else
    echo "La carpeta 'pruebas' ya existe."
fi

if [ ! -d "pruebasVacias" ]; then
    # Si no existe, crearla
    mkdir -p "$TEST_EMPTY_DIR"
    echo "Carpeta 'pruebas' creada."
else
    echo "La carpeta 'pruebas' ya existe."
fi


#genera los archivos necesarios
echo "this file has been inserted in first position" > "$TEST_DIR/$TEST_FIRST_FILE" ; seq 1 100 >> "$TEST_DIR/$TEST_FIRST_FILE"
echo "Hello how...This is  a new test" > "$TEST_DIR/$TEST_FILE" ; seq 1 1000 >> "$TEST_DIR/$TEST_FILE"
touch "$TEST_DIR/$TEST_EMPTY_FILE"


chmod g+rwx "$TEST_DIR/$TEST_FIRST_FILE"
ls -l $TEST_DIR/$TEST_FIRST_FILE

./mysha256-repo I "$TEST_DIR/$TEST_FIRST_FILE" "$REPO_FILE"
if [ $? -eq 0 ]; then
    echo "generacion del repo exitosa"
else
    echo "Error al insertar el primer archivo generando el repo"
    exit 1
fi

# Eliminar el archivo de prueba del directorio de pruebas
rm -f "$TEST_DIR/$TEST_FIRST_FILE"
echo "Archivo de prueba eliminado del directorio pruebas."

################## 1. Probar la inserción de un archivo ##################
echo " "
echo " "
echo " "
echo " 1. Probar la inserción de un archivo "

./mysha256-repo I "$TEST_DIR/$TEST_FILE" "$REPO_FILE"
if [ $? -eq 0 ]; then
    echo "Inserción de archivo único exitosa."
else
    echo "Error al insertar archivo único."
    exit 1
fi

################## 2. Probar la inserción de un archivo vacio ##################
echo " "
echo " "
echo " "
echo " 2. Probar la inserción de un archivo vacio "

./mysha256-repo I "$TEST_DIR/$TEST_EMPTY_FILE" "$REPO_FILE"
if [ $? -eq 0 ]; then
    echo "Inserción de archivo único vacio exitosa."
else
    echo "Error al insertar archivo único vacio."
    exit 1
fi


echo "verificacion visual del contenido"
hexdump  -C "$REPO_FILE"

################## 3. Extraer un archivo en 1era posicion y verificar la creacion correcta con sus permisos ##################
echo " "
echo " "
echo " "
echo "3. Extraer un archivo en 1era posicion y verificar la creacion correcta con sus permisos "

./mysha256-repo E "$TEST_DIR/$TEST_FIRST_FILE" "$REPO_FILE"
if [ $? -eq 0 ]; then
    echo "Extracción de archivo en 1era posicion exitosa."
else
    echo "Error al extraer archivo en 1era posicion."
    exit 1
fi

echo "permisos despues de la extraccion"
PERMISSIONS_EXTRACTED=$(stat -c %a "$TEST_DIR/$TEST_FIRST_FILE")
echo $PERMISSIONS_EXTRACTED

################## 4. Extraer un archivo y verificar la extracción correcta en la posicion central sobreescritura correcta de permisos ##################

echo " "
echo " "
echo " "
echo "4. Extraer un archivo y verificar la extracción correcta en la posicion central sobreescritura correcta de permisos "

echo "permisos de f1.dat contenido dentro del repositorio"
PERMISSIONS_ORIGINAL=$(stat -c %a "$TEST_DIR/$TEST_FILE")
echo $PERMISSIONS_ORIGINAL

chmod u+x "$TEST_DIR/$TEST_FILE"

echo "permisos en f1.dat del directorio"
PERMISSIONS_CHANGED=$(stat -c %a "$TEST_DIR/$TEST_FILE")
echo $PERMISSIONS_CHANGED

./mysha256-repo E "$TEST_DIR/$TEST_FILE" "$REPO_FILE"
if [ $? -eq 0 ]; then
    echo "Extracción de archivo en umtima posicion exitosa."
else
    echo "Error al extraer archivo en umtima posicion."
    exit 1
fi

echo "permisos despues de la extraccion"
PERMISSIONS_EXTRACTED=$(stat -c %a "$TEST_DIR/$TEST_FILE")
echo "$PERMISSIONS_EXTRACTED"

if [ "$PERMISSIONS_ORIGINAL" = "$PERMISSIONS_EXTRACTED" ]; then
    echo "Los permisos coinciden correctamente."
else
    echo "Los permisos no coinciden."
    exit 1
fi

################## 6. Verificar que los permisos del archivo extraído coincidan ##################
#echo "Verificando permisos del archivo extraído..."
#PERMISSIONS_ORIGINAL=$(stat -c %a "$TEST_DIR/$TEST_FILE")
#PERMISSIONS_EXTRACTED=$(stat -c %a "$TEST_FILE")
#if [ "$PERMISSIONS_ORIGINAL" = "$PERMISSIONS_EXTRACTED" ]; then
#    echo "Los permisos coinciden correctamente."
#else
#    echo "Los permisos no coinciden."
#    exit 1
#fi


################## 5. Probar la inserción de un directorio (todos los archivos dentro de él) ##################

echo " "
echo " "
echo " "
echo " 6. Probar la inserción de un directorio (todos los archivos dentro de él)"

./mysha256-repo I "$TEST_DIR" "$REPO_DIR_FILE"
if [ $? -eq 0 ]; then
    echo "Inserción de directorio exitosa."
else
    echo "Error al insertar directorio."
    exit 1
fi

################## 6. Probar la inserción de un directorio (vaciol) ##################
echo " "
echo " "
echo " "
echo " 7. Probar la inserción de un directorio (vacio) "
rm -f pruebas/*
ls -l "$REPO_DIR"
./mysha256-repo I "$TEST_DIR" "$REPO_DIR_FILE"
if [ $? -eq 0 ]; then
    echo "Inserción de directorio vacio exitosa."
else
    echo "Error al insertar directorio vacio."
    exit 1
fi


#final
echo "TODAS LAS PRUEBAS HAN FINALIZADO EXITOSAMENTE"

