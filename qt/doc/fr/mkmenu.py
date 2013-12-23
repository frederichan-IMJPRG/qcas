#!/usr/bin/env python
# -*- coding: utf-8 -*-

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
               
           if header1!=L[0] and header1 !="":
                 #output=output+"\n</ul>\n"   #manque le dernier 
                 output=output+'<li>'+current[-1]+':<br>\n'+mots+"</li>\n</ul>\n"
                 header1=""

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
              keyword=L[ikew].replace("\n","").replace("__","_")
              keyword=(keyword.replace("<","&lt;")).replace(">","&gt;")
              if keyword != ":=":
                 klist=keyword.split(":")
              else:
                 klist=[keyword]
              keyword1='<a href="'+(klist[0]).replace("(","").replace(")","")+'">'+'?'+'</a>'
              keyword1=keyword1+' <a href="!'+(klist[0]).replace("(","").replace(")","")+'">'+klist[0]+'</a>'
              keyword2=''
              if (len(klist)>1):
                 keyword2=': <font size="-1">'    
                 for jjj in klist[1:]:
                   keyword2=keyword2+jjj
                 keyword2=keyword2+"</font>"
              if(current == L[0:ikew]):
                  if mots != "":
                     mots=mots+", "
                  mots=mots+keyword1+keyword2
              else:
                  
                  if(len(current)>1):
                     titre=current[1]
                     if(len(current)>2):
                        for jj in current[2:]:
                           titre=titre+"/"+jj
                     #print '<li>'+current[-1]+':<br>\n'+mots+"</li>\n"
                     #output=output+'<li>'+current[-1]+':<br>\n'+mots+"</li>\n"
                     output=output+'<li>'+titre+':<br>\n'+mots+"</li>\n"
                  else:
                      #print '<li><font size="-1"'+mots+"</li></font>\n"
                      output=output+'<li><font size="-1"'+mots+"</li></font>\n"
                  current=L[0:ikew]
                  mots=keyword1+keyword2
                
       output=output+"\n</body> </html>"
     fhtml.write(output)

################################################ 
