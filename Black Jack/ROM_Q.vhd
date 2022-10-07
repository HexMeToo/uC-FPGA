library ieee;
use ieee.std_logic_1164.all;

entity ROM_Q is
  port ( A : in std_logic_vector(2 downto 0);
         Q : out std_logic_vector(7 downto 0));
end entity ROM_Q;

architecture Behavioral of ROM_Q is
type M is array ( 0 to 7) of std_logic_vector(7 downto 0);
constant ROM : M := (	 0  => "00000000",
								 1  => "00000000",
								 2  => "00000000",
								 3  => "00000000",
								 4  => "00000000",
								 5  => "00000000",
								 6  => "00000000",
								 7  => "00000000");
--constant ROM : M := (	 0  => "00000000",
--								 1  => "01111000",
--								 2  => "11111100",
--								 3  => "10000100",
--								 4  => "10001110",
--								 5  => "11111110",
--								 6  => "01111010",
--								 7  => "00000000");
								 
begin
	process (A)
	begin
		case A is
		when "000" => Q <= ROM(0);
		when "001" => Q <= ROM(1);
		when "010" => Q <= ROM(2);
		when "011" => Q <= ROM(3);
		when "100" => Q <= ROM(4);
		when "101" => Q <= ROM(5);
		when "110" => Q <= ROM(6);
		when "111" => Q <= ROM(7);
		end case;
	end process;

end architecture Behavioral;