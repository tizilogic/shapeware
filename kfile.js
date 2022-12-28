let project = new Project('shapeware');

project.addFile('Sources/**');
project.addIncludeDir('Sources');
if (typeof noKrinkPlease === 'undefined') {
    await project.addProject('krink');
    project.setDebugDir('Deployment');
}
else project.addExclude('Sources/dummy.c');

project.addFile('ext/sht/sht.c');
project.addFile('ext/sht/murmur3.c');
project.addIncludeDir('ext');
project.flatten();
resolve(project);
