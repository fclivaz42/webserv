#!/bin/bash

# Adresse et port du serveur
SERVER_ADDRESS="localhost"
SERVER_PORT="3000"

# Répertoire de test
TEST_DIR="../public/delete/"

# Création du répertoire de test s'il n'existe pas
mkdir -p $TEST_DIR

# Fichiers de test
FILES=("test1.txt" "test2.txt" "no_permission.txt")

# Création de fichiers de test dans le répertoire `public/delete/`
echo "Fichier de test 1" > $TEST_DIR/test1.txt
echo "Fichier de test 2" > $TEST_DIR/test2.txt
echo "Fichier sans permission" > $TEST_DIR/no_permission.txt

# Changer les permissions du fichier "no_permission.txt" pour qu'il ne puisse pas être supprimé
chmod 444 $TEST_DIR/no_permission.txt
echo "Server address: $SERVER_ADDRESS"

# Test 1 : Suppression d'un fichier existant
echo "Test 1 : Suppression d'un fichier existant"
curl -X DELETE http://$SERVER_ADDRESS:$SERVER_PORT/delete/test1.txt -v
echo -e "\n"

# Test 2 : Tentative de suppression d'un fichier inexistant
echo "Test 2 : Suppression d'un fichier inexistant"
curl -X DELETE http://$SERVER_ADDRESS:$SERVER_PORT/delete/inexistant.txt -v
echo -e "\n"

# Test 3 : Tentative de suppression d'un fichier sans permissions
echo "Test 3 : Suppression d'un fichier sans permissions"
curl -X DELETE http://$SERVER_ADDRESS:$SERVER_PORT/delete/no_permission.txt -v
echo -e "\n"

# Nettoyage des fichiers de test
rm $TEST_DIR/test2.txt
chmod 644 $TEST_DIR/no_permission.txt  # Restaurer les permissions avant suppression
rm $TEST_DIR/no_permission.txt

echo "Tests terminés."