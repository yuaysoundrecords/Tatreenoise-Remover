# Tatree 2.1 Gumroad handoff — not uploaded

Product: https://benjilennon.gumroad.com/l/rsecg . This is Tatree, not Prued FX (`rkpck`).

The owner requests a free 30-minute trial per DAW session, unlimited restarts, and US$5 paid keys for uninterrupted processing. All instances in the same host process share the timer. Expiry fades to latency-matched dry. Saved keys need an internet check once per new host process; no audio/project data is sent.

## Current blocker

The public product ID has not been obtained. This cloud's outbound policy currently blocks Gumroad. A draft adds `gumroad.com`, `benjilennon.gumroad.com` and `api.gumroad.com`; reviewing/saving/publishing that draft is required before trying those destinations again. Network access alone does not provide authenticated seller access.

No authenticated browser or Gumroad product-editing connector is available. No price, file, key-delivery setting or KVR page has been changed by this environment. User-reported Prued FX updates do not establish Tatree configuration.

## Release order

1. Enable license keys on the **Tatree** product and set the paid price/minimum to **US$5**. Provide a free trial download separately, linking the same binaries. Avoid implying that a free receipt unlocks the paid edition.
2. Configure the public `TATREE_GUMROAD_PRODUCT_ID` build/Actions variable. Never use a seller access token as that value.
3. Build both platforms, pass native tests, and verify one real US$5+ paid key against this exact product. A staging package with missing ID cannot be sold as unlockable. Existing 2.0.4 artifacts have no trial or key system and are not the new paid release.
4. Upload both new platform ZIPs (extract the outer GitHub artifact ZIP first). Use Windows / Mac labels and include the Thai-then-English installation/use text, followed by the trial/key text in [LICENSING.md](LICENSING.md).
5. Only after both uploads succeed, remove **all older customer-download files** from this product, keeping recovery copies outside the customer list. Save and check the buyer-facing download list.

Mac still has an ad-hoc signature and is not Apple-notarized. Do not advertise a notarized installer. No subscriber email, pricing change on another product, or customer-key publication is part of this handoff.
