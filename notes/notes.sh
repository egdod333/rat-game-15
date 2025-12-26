LAST_EDIT=$(date -r "notes" +%s)
LAST_READ=$(date -d "$(head -n 1 "notes")" +%s)
if test $LAST_EDIT -gt $LAST_READ;then
  echo "you've got mail"
  less notes
  sed -i "1c\\$(date)" notes
  touch -d "$(date -d @$LAST_EDIT)" notes
  exit 1
fi
exit 0