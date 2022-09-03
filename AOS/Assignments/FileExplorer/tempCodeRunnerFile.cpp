string ResolvePath(string path){
//     switch(path[0]){
//         case '/':
//             //here we are expecting the give path is absolute 
//             return path;
//             break;
//         case '~':
//             return (getHome() + path.substr(1));
//             break;
//         case '.':
//             //checking if it is ..
//             if(path[1]=='.')
//                 return (getHome() + path.substr(2));
//             return (getHome() + path.substr(1));
//             break;
//         default:

//             break;
//     }

// }