For at kunne bruge ssh clone, skal der først oprettes en ssh key til account. 
Følg guiden her [Gitlab Guide](https://docs.gitlab.com/ee/ssh/)


# Guide til at få git (Simple kommandoer)
### Sådan downloader du filer til din computer
1. `git clone git@gitlab.au.dk:sharks/project-phoenix.git <fil placering>`\
    Jeg synes det er nemmer at navigerer til mappen og derefter bruge . (punktum)
### Sådan uploader du filer til GitLab
1. `git add -<filnavn eller A for alt>`\
    Tilføjer filen til staging areaet
2. `git commit -m "Beskrivelse af ændringer"`\
    Klar gør til at blive sendt
3. `git pull origin <branch navn>`\
    Henter evt. nye ændringer til filerne
4. `git push origin <branch navn>`\
    Uploader filer til GitLab

### Sådan laver du en branch
1. `git branch <navn>`\
    Laver en ny branch
2. `git branch`\
    Viser alle branches
3. `git checkout <branch navn>`\
    Sådan skifter man mellem branches 
4. `git push -u origin <branch navn>`\
    Uploader din branch til GitLab

### Sådan merger du en branch 
1. `git checkout master`\
    Skifter til master branch
2. `git pull origin master`\
    Henter ændringer
3. `git merge <branch navn>`\
    Fletter branches
4. `git push origin master`\
    Uploader til GitLab

### Sådan sletter du en branch 
1. `git branch -d <branch navn>`\
   Sletter branchen lokalt
2. `git push origin --delete <branch navn>`\
   Sletter branchen på GitLab

## Sådan fixer du en merge konfilkt
1. Ingen ved det
