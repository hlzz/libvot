## Roadmap (Version 0.2)

### Version 0.2 will concentrate on these issues: 
* Improvement: support image database update

* Detailed unit test for each module

* Deep learning methods support (tensorflow)

* [Hashing methods](https://github.com/willard-yuan/hashing-baseline-for-image-retrieval) support

* Re-organize code structure  
    - better cmake organization (add installation rules)

* Add more feature support, such as VLFeat, BRIEF, etc.  
    - add vlfeat (SIFT)

* benchmark dataset performance (2D / 3D)
    - [Oxford 5k](http://www.robots.ox.ac.uk/~vgg/data/oxbuildings/)
    - [Flickr 1M](http://press.liacs.nl/mirflickr/)

* SLAM-friendly development, add loop closure detection

* Better engineering  
    - command line system (e.g. gflags)  
    - doxygen documentation
    - python export

* Add option to support opencv
    - image I/O currently rely on opencv
    - opencv feature support (contains non-free modules)