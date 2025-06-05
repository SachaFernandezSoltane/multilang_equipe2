# 🌍 Projet Multilang

## 📚 Avant de commencer

👉 **Veuillez lire le `README.md` de chaque projet individuel avant de lancer son conteneur Docker.**  
Chaque implémentation peut contenir des instructions spécifiques ou des dépendances particulières à respecter.

---

Bienvenue dans **Projet Multilang**, une démonstration élégante et pédagogique d'un même algorithme traduit dans de nombreux langages de programmation modernes. Ce projet met en lumière la diversité syntaxique et les paradigmes de différents langages tout en conservant une logique algorithmique identique.

## 🚀 Objectif

L'objectif de ce projet est de proposer une implémentation rigoureusement équivalente d'un algorithme dans plusieurs langages populaires, afin de :

- Comparer la syntaxe et la structure du code selon les langages
- Illustrer les différences de paradigmes (impératif, fonctionnel, orienté objet, etc.)
- Servir de référence pour les développeurs curieux ou en formation

## 🔤 Langages pris en charge

Le même algorithme a été soigneusement traduit dans les langages suivants :

- ✅ **C**
- ✅ **C#**
- ✅ **C++**
- ✅ **Go**
- ✅ **Java**
- ✅ **Kotlin**
- ✅ **PHP**
- ✅ **Python**
- ✅ **Scala** *(via `scala-cli`, **non dockerisé**)*
- ✅ **Haskell** *(via `stack`, **non dockerisé**)*
- ✅ **TypeScript**

## 🐳 Dockerisation

La majorité des implémentations sont **entièrement dockerisées**, ce qui permet de les exécuter sans installer chaque environnement localement.

> ⚠️ **Scala** et **Haskell** ne sont pas dockerisés à ce stade. Ils nécessitent respectivement :
>
> - `scala-cli` pour Scala
> - `stack` pour Haskell

### 📦 Construction d'une image Docker

```bash
docker build -t ssp-<langage> .
```

```bash
docker run -v "$PWD/../instances:/data" ssp-<langage> /data/<nom du fichier>
```

### 📦 Utiliser stack pour Haskell

Voir le readme du projet Haskell

### 📦 Utiliser scala-cli pour Scala

Voir le readme du projet Scala


"# multilang_equipe2" 
