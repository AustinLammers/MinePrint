# MinePrint Development Notes
- User Experience
  - We want the user to beable to place two blocks in oposing corners
    - Idealy, the front right corner, and far right corner for example
    - Once the user has done this, we want to only grab the content within the bounds of the cube defined by these blocks
      - Export this data to a seperate file that can be read by another program
- STL Generator
  - Once we have exported world data, a secondary application will read the file and generate the STL from the world data exported by our mod
    - Mod could exec this secondary application