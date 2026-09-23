# Coffee beverage images

The drink and brewing PNGs were generated for this example with the built-in
image-generation tool on 2026-09-20. They are original project artwork; no
third-party product photography is included. GSPC scales the gallery and recipe images at `store_scale: 0.32` with
`fit: cover`. Brewing cutouts use `store_scale: 0.2` and `fit: contain`; the
controller selects the drink-specific image.

## Drink images

- `coffee_hero.png`: cappuccino cup and saucer on a warm cream background.
- `coffee_espresso.png`: espresso demitasse and saucer with golden crema.
- `coffee_latte.png`: layered latte in a clear glass on a warm-white saucer.

## Brewing cutouts

`coffee_brew.png`, `coffee_brew_espresso.png` and `coffee_brew_latte.png` are
generated transparent drink cutouts. Native GSP shimmer and ring effects
provide motion separately from the images; source alpha is retained and the
scene uses compiler store scaling. The controller displays the cutout
corresponding to the selected beverage.
