/* Hasq technology 2015 */

function get_current_hash_func()
{
    var x = $("#engine").val();

    if ( x == "hex_md5" ) return hex_md5;
    if ( x == "sha512" ) return hex_sha512;
    if ( x == "sha256" ) return hex_sha256;
    if ( x == "sha1" ) return hex_sha1;
    if ( x == "rmd160" ) return hex_rmd160;

    return hex_md5;
}

var keyth_text = true;
function keyth_clicked()
{
    var x = $("#key1").prop("checked");
    if ( x && keyth_text ) return;
    if ( !x && !keyth_text ) return;

    keyth_text = !keyth_text;

    if ( keyth_text ) key_hex_to_text();
    else key_text_to_hex();
}

$(function()
{
    $('input.keyth').click( function( event )
    {
        keyth_clicked();
    })
} );


var ascii = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
var basc = new Object();

function initascii()
{
    for ( var i = 0; i < ascii.length; i++ )
    {
        basc[ascii.charAt(i)] = i;
    }
}

initascii();

function char2int(x)
{
    if ( x == '\n' ) return 13;
    if ( x == ' ' ) return 32;
    if ( !basc[x] ) return 0;
    return basc[x] + 32;
}

function int2char(x)
{
    if ( x == 10 ) return "\n";
    if ( x == 13 ) return "\n";
    if ( x < 32 ) return ".";
    if ( x > 126 ) return ".";
    if ( x == 32 ) return ' ';
    return ascii.charAt(x - 32);
}

function hexchar2int(x)
{
    var y = char2int(x);
    if ( y >= 48 && y <= 57 ) return y - 48;
    if ( y >= 65 && y <= 70 ) return y + 10 - 65;
    if ( y >= 97 && y <= 102 ) return y + 10 - 97;
    return -1;
}

function int2hexchar(x)
{
    if ( x <= 9 ) return int2char(x + 48);
    if ( x <= 15 ) return int2char(x - 10 + 97);
    return int2char(0);
}

function hex_to_text(x)
{
    var y = "";
    var b = 0;
    var half = false;

    for ( var i = 0; i < x.length; i++ )
    {
        var c = x.charAt(i);
        c = hexchar2int(c);
        if ( c < 0 ) continue;

        if ( half )
        {
            y += int2char(b + c);
            half = false;
        }
        else
        {
            b = 16 * c;
            half = true;
        }

    }

    return y;
}

function hex_to_raw(x)
{
    var y = "";
    var b = 0;
    var half = false;

    for ( var i = 0; i < x.length; i++ )
    {
        var c = x.charAt(i);
        c = hexchar2int(c);
        if ( c < 0 ) continue;

        if ( half )
        {
            y += String.fromCharCode(b + c);
            half = false;
        }
        else
        {
            b = 16 * c;
            half = true;
        }

    }

    return y;
}

function key_hex_to_text()
{
    var x = $("#key").val();
    var y = hex_to_text(x);
    $("#key").val(y);
}

function text_to_hex(x)
{
    x = rstr2hex(x);
    return x;
}

function key_text_to_hex()
{
    var x = $("#key").val();
    x = text_to_hex(x);
    $("#key").val(x);
}


function randomize(data)
{
    var nostop = $("#rnd").is(':checked');
    if ( !nostop ) return;

    var iv_text = $("#iv_text");
    var salt_text = $("#salt_text");
    var iv_text_str = iv_text.val() + data;

    var hfunc = get_current_hash_func();

    iv_text_str = hfunc(iv_text_str);

    iv_text.val(iv_text_str);
    salt_text.val(hfunc(salt_text.val() + data));
}

function init_random()
{
    $( "#iv_text" ).val("18ac3e7343f016890c510e93f935261169d9e3f565436429830faf0934f4f8e4");
    $("#salt_text").val("3e23e8160039594a33894f6564e1b1348bbd7a0088d42c4acb73eeaed59c009d");

    $(document).mousemove(function(event)
    {
        randomize(" " + event.pageX + " " + event.pageY);
    });

    randomize("");

    setInterval(function() {randomize("");}, 1000);
}




function window_resize()
{
    var text = $("#text");
    var key = $("#key");
    var opts = $("#opts");
    var pos_text = text.position();

    var maxy = $(document).height();
    var maxx = $(document).width();
    var wh = $(window).height();

    var text_hoff = 500;
    var new_th = maxy - text_hoff;
    if ( new_th < 100 ) new_th = 100;
    text.height( new_th );

    var text_woff = 50;
    var new_tw = maxx - text_woff;
    if ( new_tw < 100 ) new_tw = 100;
    text.width( new_tw );

    var key_height = 150;
    key.height( key_height );

    var key_woff = 50;
    var key_width = new_tw - opts.width() - key_woff;
    if ( key_width < 100 ) key_width = 100;
    key.width( key_width );
}


function init_resize()
{
    $( window ).resize(function() { window_resize(); });
    window_resize();
}

function breaklines(a, len)
{
    var sz = a.length;
    var c = "";

    for ( var i = 0; i < sz; i++ )
    {
        if ( i > 0 && (i % len) == 0 ) c += '\n';
        c += a.charAt(i);
    }

    return c;
}


function hjdbc_encrypt()
{
    var t = $("#text").val();

    if ( !is_ascii(t) )
    {
        alert("The text is not plain ASCII. "
              + "Convert it first to ASCII by clicking button 'Uni->ASCII'.");
        return;
    }

    var iv = $("#iv_text").val() + "";
    var salt = $("#salt_text").val() + "";
    var H = get_current_hash_func();
    var blk_size = H("").length;

    if ( iv.length != salt.length )
    {
        alert("IV and Salt sizes do not match");
        return;
    }

    if ( iv.length != blk_size )
    {
        alert("IV size does not match Digest function");
        return;
    }

    var add_sign = $("#ver_block").is(':checked');
    var min_salt = $("#min_salt").val();
    var min_salt = parseInt(min_salt);

    // 1 add size
    t += ":" + t.length;
    t = text_to_hex(t);
    var tsz = t.length;

    // 2 pad with salt
    var psz = tsz % blk_size;
    psz = blk_size - psz;

    if ( psz < min_salt )
    {
        t = salt + t;
        tsz = t.length;
    }

    t = salt.substr(0, psz) + t;
    tsz = t.length;

    // 3 convert to blocks
    var v = Array();
    var N = tsz / blk_size;
    for ( var i = 0; i < N; i++ )
    {
        v[i] = t.substr(i * blk_size, blk_size);
    }
    var C = hjdbc_enc_cipher(v, iv, H);

    N = C.length;
    if ( !add_sign ) --N;
    var c_text = iv + '\n';
    for ( var i = 0; i < N; i++ )
    {
        c_text += C[i] + '\n';
    }

    if ( $("#base64").is(':checked') )
    {
        c_text = Base64.encode(hex_to_raw(c_text));
        var l80 = parseInt($("#base64_line").val());
        if ( l80 < 4 || l80 > 1000 ) l80 = 80;
        c_text = breaklines(c_text, l80);
    }

    $("#auth").html("");
    $("#text").val(c_text);

}


function hex_filter(x)
{
    var r = "";
    for ( var i = 0; i < x.length; i++ )
    {
        var k = hexchar2int(x.charAt(i));
        if ( k >= 0 && k < 16 ) r += x.charAt(i);
    }

    return r;
}

function hjdbc_decrypt()
{
    var t = $("#text").val();
    var H = get_current_hash_func()
            var blk_size = H("").length;

    if ( $("#base64").is(':checked') )
        t = text_to_hex(Base64.decode(t));

    t = hex_filter(t);

    var v = Array();
    var N = t.length / blk_size;

    for ( var i = 0; i < N; i++ )
    {
        v[i] = t.substr(i * blk_size, blk_size);
    }

    var P = hjdbc_dec_cipher(v, H);

    N = P.length;
    var p_text = "";
    for ( var i = 0; i < N; i++ )
    {
        p_text += P[i] + '\n';
    }

    p_text = hex_to_text(p_text);

    var n = p_text.lastIndexOf(":");
    if ( n < 0 )
    {
        $("#auth").html("Invalid data");
        return;
    }

    var msg_sz = parseInt( p_text.substr(n + 1) );

    if ( msg_sz < 0 || msg_sz > n )
    {
        $("#auth").html("Invalid data");
        return;
    }

    var sz = p_text.length;
    var msg = p_text.substr(n - msg_sz, msg_sz);

    var auth = "Authentication: ";

    if ( sz - n > blk_size / 2 )
        auth += "INVALID";
    else if ( v.length == P.length + 1 )
        auth += "absent";
    else if ( v.length == P.length + 2 )
        auth += "Valid";
    else
        auth += "Cipher ERROR " + v.length + " " + P.length;

    $("#auth").html(auth);

    $("#text").val(msg);

}

function init_cipher()
{
    var x = $("hjdbc_encrypt");
    $("#hjdbc_encrypt").click(hjdbc_encrypt);
    $("#hjdbc_decrypt").click(hjdbc_decrypt);
    $("#hjdbc_debug").click(hjdbc_debug);

    $("#b_uni2ascii").click(b_uni2ascii);
    $("#b_ascii2uni").click(b_ascii2uni);

    $("#simple_encrypt").click(simple_encrypt);
    $("#simple_decrypt").click(simple_decrypt);
}

function hex_xor(a, b)
{
    var sz = b.length;
    if ( a.length != sz ) return "";

    var c = "";

    for ( var i = 0; i < sz; i++ )
    {
        var ia = hexchar2int(a.charAt(i));
        var ib = hexchar2int(b.charAt(i));
        c += int2hexchar( ia ^ ib );
    }

    return c;
}

function get_key()
{
    var K = $("#key").val();
    if ( $("#key1").prop("checked") ) return K;
    return hex_to_raw(K);
}

function hjdbc_enc_cipher(v, iv, H)
{
    var K = get_key();
    var C = Array();

    var cur = iv;
    for ( var i = 0; i < v.length; i++ )
    {
        var x = H(K + cur);
        cur = hex_xor(x, v[i]);
        C[i] = cur;
    }

    C[v.length] = H(K + cur);

    return C;
}

function hjdbc_dec_cipher(v, H)
{
    var K = get_key();
    var P = Array();

    for ( var i = 1; i < v.length; i++ )
    {
        var x = H(K + v[i - 1]);
        if ( i + 1 == v.length && x == v[i] ) break;
        P[i - 1] = hex_xor(x, v[i]);
    }

    return P;
}

function is_ascii(x)
{
    for ( var i = 0; i < x.length; i++ )
    {
        var k = int2char(char2int(x.charAt(i)));
        if ( k != x.charAt(i) ) return false;
    }

    return true;
}

//var tt = "";
function hjdbc_debug()
{
    if (0)
    {
        var t2 = "0d0a";

        var a0 = hex_to_raw(t2);
        var a1 = Base64.encode(a0);
        var a5 = Base64.decode(a1);
        var a6 = text_to_hex(a5);

        var e = "";
        if ( a0 != a5 ) e = "ERROR";

        $("#dbg").html("hjdbc_debug:" + e + " " + t2 + " " + a6);
    }

    if ( tt == "" )
    {
        tt = $("#text").val();
        if ( !is_ascii(tt) )
        {
            alert("The text is not plain ASCII. "
                  + "Convert it first to ASCII by clicking button 'Uni->ASCII'.");
            tt = "";
            return;
        }
        tt = text_to_hex(tt);
        $("#text").val(tt);
        return;
    }

    tt = hex_to_raw(tt);
    $("#text").val(tt);
    tt = "";

}

function b_uni2ascii()
{
    var t = $("#text").val();
    t = uni_to_ascii(t);
    $("#text").val(t);
}

function b_ascii2uni()
{
    var t = $("#text").val();
    t = ascii_to_uni(t);
    $("#text").val(t);
}

function uni_to_ascii(x)
{
    var r = "";
    for ( var i = 0; i < x.length; i++ )
    {
        var k = int2char(char2int(x.charAt(i)));
        if ( k == x.charAt(i) )
            r += k;
        else
        {
            var code = "" + x.charCodeAt(i);
            while ( code.length < 4 ) code = "0" + code;
            r += "\\u" + code;
        }
    }

    return r;
}

function ascii_to_uni(x)
{
    var r = /\\u([\d\w]{4})/gi;
    x = x.replace(r, function (match, grp)
    {
        return String.fromCharCode(parseInt(grp, 10));
    } );
    x = unescape(x);
    return x;
}

function simple_encrypt()
{
    // copy passwd to key
    var pwd = $("#passwd").val();
    $("#key").val(pwd);
    b_uni2ascii();
    hjdbc_encrypt();
}

function simple_decrypt()
{
    var pwd = $("#passwd").val();
    $("#key").val(pwd);
    hjdbc_decrypt();
    b_ascii2uni();
}
