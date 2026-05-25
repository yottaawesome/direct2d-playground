# TODO

- [ ] **DrawScene calls CreateResources + Load every frame** (mainapp.ixx:131-132). Both are guarded no-ops, but they belong in the constructor, not the hot path.
- [ ] **MaximumSpriteCount = 5 is too low** (entity.ixx:26). Player + 4 enemies exhaust all slots; no room for bullets.
- [ ] **DrawBitmap dest rect is inverted** (mainapp.ixx:156). `D2D1::RectF(100, 100, 0, 0)` has left > right and top > bottom — won't render correctly.
- [ ] **PositionPlayer swaps X/Y** (mainapp.ixx:210-211). `rect.bottom` (Y) assigned to `playerX`, `rect.right` (X) assigned to `playerY`.
- [ ] **Player hardcoded to index `[0]` in HandleKeyDown** (mainapp.ixx:260). Should find the player entity dynamically.
- [ ] **Unused InputState/InputState2/KeysPressedMap** defined in mainapp.ixx but never wired in.
- [ ] **Empty wiccomponents.ixx module** has no exported content.
