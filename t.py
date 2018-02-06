file = open("title.basics.justMovies.csv","r") 
A = file.read().split("\n")

filePrin = open("title.principals.csv","r")
B = filePrin.read().split("\n")

sizeA = len(A)
sizeB = len(B)

ia = 0
ib = 0

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

