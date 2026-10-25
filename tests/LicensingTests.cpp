#include "Licensing.h"
#include <iostream>
#include <stdexcept>

static void check(bool ok,const char* message) {
    if(!ok) throw std::runtime_error(message);
    std::cout << "PASS " << message << '\n';
}
static juce::String response(const juce::String& id="tatree-product",int cents=500,
                            bool refunded=false,bool disputed=false,bool test=false,
                            juce::String currency="usd") {
    auto purchase=std::make_unique<juce::DynamicObject>();
    purchase->setProperty("product_id",id); purchase->setProperty("price",cents);
    purchase->setProperty("currency",currency); purchase->setProperty("refunded",refunded);
    purchase->setProperty("disputed",disputed); purchase->setProperty("test",test);
    auto root=std::make_unique<juce::DynamicObject>();
    root->setProperty("success",true); root->setProperty("purchase",juce::var(purchase.release()));
    return juce::JSON::toString(juce::var(root.release()));
}
int main() {
    using V=TatreeLicense::Verdict;
    try {
        check(SessionTrial::remaining(50,50)==1800,"trial starts at thirty minutes");
        check(SessionTrial::remaining(50,1849)==1,"trial retains final second");
        check(SessionTrial::remaining(50,1850)==0,"trial expires at exactly thirty minutes");
        check(SessionTrial::remaining(50,9000)==0,"expired trial never becomes negative");
        check(SessionTrial::remaining(50,0)==1800,"clock anomaly cannot extend trial past thirty minutes");
        const auto shared=TatreeLicense::shared();
        check(shared.get()==TatreeLicense::shared().get(),"all instances share one session and activation");
        const auto a=shared->secondsRemaining();
        { auto another=TatreeLicense::shared(); }
        check(shared->secondsRemaining()<=a,"removing an instance does not reset the clock");
        const auto verify=[](int status,const juce::String& body){return TatreeLicense::validateResponse(status,body,"tatree-product");};
        check(verify(200,response())==V::accepted,"valid five-dollar Tatree purchase unlocks");
        check(verify(200,response("tatree-product",750))==V::accepted,"paying more than five dollars unlocks");
        check(verify(200,response("tatree-product",0))==V::rejected,"free-download key never unlocks");
        check(verify(200,response("tatree-product",499))==V::rejected,"payment below five dollars never unlocks");
        check(verify(200,response("another-product"))==V::rejected,"another product key never unlocks");
        check(verify(200,response("tatree-product",500,true))==V::rejected,"refunded purchase is rejected");
        check(verify(200,response("tatree-product",500,false,true))==V::rejected,"disputed purchase is rejected");
        check(verify(200,response("tatree-product",500,false,false,true))==V::rejected,"test purchase never unlocks production");
        check(verify(200,response("tatree-product",500,false,false,false,"thb"))==V::rejected,"wrong currency never unlocks");
        check(verify(200,"{\"success\":true}")==V::rejected,"success without a matching purchase is rejected");
        check(verify(200,"{\"success\":\"true\"}")==V::unavailable,"string truth value is not trusted");
        check(verify(200,"<html>maintenance</html>")==V::unavailable,"HTML or invalid JSON never unlocks");
        check(verify(0,"")==V::unavailable,"network failure never unlocks");
        check(verify(429,"{\"success\":false}")==V::unavailable,"rate limit is not treated as key revocation");
        check(verify(503,"{\"success\":false}")==V::unavailable,"server fault is not treated as key revocation");
        check(verify(404,"{\"success\":false}")==V::rejected,"explicit missing key is rejected");
        check(TatreeLicense::validateResponse(200,response(),"")==V::unavailable,"unconfigured build cannot activate");
        check(verify(200,juce::String::repeatedString("x",65537))==V::unavailable,"oversized response is bounded");
        std::cout << "Licensing policy checks passed\n";
        return 0;
    } catch(const std::exception& e) {std::cerr << "FAIL " << e.what() << '\n'; return 1;}
}
