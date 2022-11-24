curl -x http://10.52.0.202:8080 -U mazonkao:xxx http://mazonka.com/box/jrafcl/ -o curl.out -D curl.head -d "command=jr read 0 /"
cat curl.out
