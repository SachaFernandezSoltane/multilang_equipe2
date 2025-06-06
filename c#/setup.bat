@echo off
REM Crée un nouveau projet console nommé SSPApp
dotnet new console -n SSPApp

REM Attendre que le projet soit bien créé
if not exist SSPApp\Program.cs (
    echo Erreur : le projet n'a pas été créé correctement.
    exit /b 1
)

REM Déplace ton fichier SSP.cs vers SSPApp en le renommant Program.cs
copy /Y Program.cs SSPApp\Program.cs

echo Projet SSPApp prêt avec SSP.cs comme Program.cs
