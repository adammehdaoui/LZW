# Projet LZW 

## Documentation technique

La documentation technique du code est dans le **fichier Doxygen.html dans le répertoire doc à la racine du projet.**
<br>
Ce document est à ouvrir **avec un navigateur** car il est chargé à partir des fichiers de configuration Doxygen. <br>

## Encodage

Pour lancer l'encodage, il convient d'utiliser le format suivant :

```sh
./LZW encode -max12 <NomFichier>
```
Tous les formats de fichiers peuvent être encodés avec LZW.
L'encodage se base sur un nombre de bits variable avec : <br>
- un minimum de 9 bits <br>
- un maximum de 12 à 20 bits

Ainsi le paramètre -max peut varier de -max12 à -max20.

Le fichier généré porte la dénomination encode_&#60;NomFichier&#62;


<br>
## Decodage

Pour lancer le décodage, il convient d'utiliser le format suivant :

```sh
./LZW decode -max12 <NomFichier>
```
Tous les fichiers encodés avec la norme LZW peuvent être décodés.

Il est obligatoire d'utiliser le même -max pour l'encodage et le décodage.

Le fichier généré porte la dénomination decode_&#60;NomFichier&#62;


