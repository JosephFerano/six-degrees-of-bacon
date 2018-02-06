file = open("title.principals.justMovies.csv","r") 
A = file.read().split("\n")

L = set()
ctr = 0
for a in A:
  if (len(a) > 2): 
	  #print "----", a
	  for e in  a.split("\"")[3].split(","):
	  	L.add(e)

	  # if ctr %10000 == 0 : print ctr
	  # ctr = ctr + 1
  
# print L

filePrin = open("name.basics.csv","r")
#B = filePrin.read().split("\n")

for name in filePrin:
  n = name.split(",")[0].replace("\"","")
  if n in L: print name,

sizeA = len(A)
sizeB = len(B)

ia = 0
ib = 0
'''
while (ia < sizeA and ib < sizeB):
  titA = A[ia].split(",")[0]
  titB = B[ib].split(",")[0]

  if titA  == titB:
    #print A[ia] 
    print B[ib] 
    ia = ia + 1
    ib = ib + 1
  elif titB < titA:
    ib = ib+1
  else:
    ia = ia+1 

'''
