# -*- coding: utf-8 -*-
"""
Created on Sat Jun 22 17:32:42 2019

@author: lexa
"""

from pathlib import Path
import os

startDir = os.getcwd()

def getIgnore():
    
    def findStrInList(list_,str_):
        found = False
        for s in list_:
            if s.find(str_)!=-1:
                found = True
        return found

    fileTypes = []
    dirs = []
    currentTag = ""
    isTag = False
    tags = ["[fileTypes]","[dirIgnore]","[output]"]
    lists = {tags[0]:fileTypes,tags[1]:dirs}
    f = open("lines_counter.ini", 'r', -1,errors='ignore',encoding = "utf-8") 
    for line in f:
        # нужно пофиксить
        if line.find('#')!=-1 or len(line)<2:
            continue
        if findStrInList(tags,line[:-1])==True:
            currentTag = line[:-1]
            isTag = True
        if len(currentTag)!=0 and isTag == False and findStrInList(lists.keys(),line[:-1]) == False:
            lists[currentTag].append(line[:-1]);
        isTag = False
    f.close()
    newDirs = [os.path.join(Path.cwd(), d[:-1]) for d in dirs]
    return fileTypes,newDirs

def in_directory(dir_, dirIgnore):       
    for directory in dirIgnore:
        if os.path.commonprefix([dir_, directory]) == directory:
            return True         
    return False

def get_path_line_count(path, fileTypes, dirIgnore, line_counter_, dir_counter):
    
    def get_file_line_count(file,fileTypes, line_counter = 0):
        for f in fileTypes:
            if file.endswith(f):
                print(file)
                f = open(file, 'r', -1,errors='ignore');
                for line in f:
                     line_counter+=1
                f.close()
                print(line_counter)
        return line_counter
      
    tree = os.walk(path)
    for root, dirs, files in tree:
        if in_directory(root,dirIgnore):
            continue
        dir_counter+=1
        for file in os.listdir(root):
            line_counter_+=get_file_line_count(root+'\\'+file,fileTypes)
    return line_counter_,dir_counter

if __name__ == "__main__":
    current_dir = Path.cwd()
    print('currentDir-> ', current_dir)
    line_counter = 0
    dir_counter = -1
    ret = getIgnore()
    line_counter,dir_counter = get_path_line_count(current_dir, ret[0],ret[1], line_counter,dir_counter)
    print ('\n', 'gj!__________________', line_counter,  ' lines,')
    print('\n','_____________________',dir_counter, 'dirs in project!')