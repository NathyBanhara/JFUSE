nDoc= int(input("Doc quantity: "))
fname=("extractTwitter.json")
#fname=("roam_prescription_based_prediction.jsonl")
#fname=("yelp_academic_dataset_business.json")
#fname=("yelp_academic_dataset_user.json")
fout=open('extractPrescrip.json', 'w')
#fout=open('novo.json','w')
fout.write('{'+'\n')
with open(fname) as f:
	lines=f.readlines()
	for i,line in enumerate(lines):
		line=line.strip()[1:-1] # remove { and }\n
		if i==nDoc:
			line+='}'
			fout.write(line+'\n')
			break
		line+=','
		fout.write(line+'\n')
fout.close()