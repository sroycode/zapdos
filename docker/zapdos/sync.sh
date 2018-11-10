rsync -aHe \
--delete \
--exclude ".git" \
--exclude ".gitignore" \
--exclude ".bin" \
--exclude "build" \
--exclude "thirdparty" \
--exclude "SHREOS" \
--exclude ".idea" \
-L \
/Users/shreos/projects/github/zapdos \
/Users/shreos/projects/github/zapdos/docker/zapdos
