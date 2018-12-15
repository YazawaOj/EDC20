inf = open('path.c', 'r',encoding='utf-8')
lines=[]

count = 0
for line in inf:
    if 'addOperation' in line and 'operationEncoder' in line:
        lines.append(str(line))
        count+=1
    else:
        continue

for l in sorted(lines):
    print(l,end='')
print(count)

input()