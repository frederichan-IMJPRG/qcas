#!/usr/bin/env python
#FICH=['xcasmenu','Cmds','Phys','Scolaire']
FICH=['xcasmenu']
for FI in FICH:
   with open(FI+".html",'w') as fhtml:
     with open("xcasmenu") as f:
       header1=""
       output='<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">\n<html>\n<head>\n<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"><title>Keywords</title>\n</head>\n<body>\n'

       for line in f:
        L=line.split("/")

        if(L[0]==FI)or(FI=="xcasmenu"):
           if(L[0]==FI): 
              L=L[1:]
               
           if header1=="":
               header1=L[0]
               current=L[0:-1]
               mots=""
               #print "<h3>"+header1+"</h3>\n<ul>"
               if len(L)>2:
                   output=output+'<h3><a name="%s">'%(L[0])+header1+"</a></h3>\n<ul>"
           if header1==L[0]:    
              d=len(L)
              ikew=-1
              if len(L)<2:
                  ikew=0
              if(current == L[0:ikew]):
                   keyword=L[ikew].replace("\n","").replace("__","_")
                   if mots=="":
                       mots='<a href="'+(keyword.split(":"))[0]+'">'+keyword+'</a>'
                   else:
                       mots=mots+', <a href="'+(keyword.split(":"))[0]+'">'+keyword+'</a>'
              else:
                  
                  if(len(current)>1): 
                     #print '<li>'+current[-1]+':<br>\n'+mots+"</li>\n"
                     output=output+'<li>'+current[-1]+':<br>\n'+mots+"</li>\n" 
                  else:
                      #print '<li><font size="-1"'+mots+"</li></font>\n"
                      output=output+'<li><font size="-1"'+mots+"</li></font>\n"
                  current=L[0:ikew]
                  mots=""
          
           else:
                 header1=L[0]
                 ikew=-1
                 if len(current)<1:
                     output=output+' '+mots+"\n</ul>\n"
                 else:
                     #print '<li>'+current[-1]+':<br>\n'+mots+"</li>\n</ul>\n"
                     output=output+'<li>'+current[-1]+':<br>\n'+mots+"</li>\n</ul>\n"
                 header1=""
       output=output+"\n</body> </html>"
     fhtml.write(output)

################################################ 
